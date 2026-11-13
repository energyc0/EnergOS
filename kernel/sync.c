#include "sync.h"

#include <stdatomic.h>

#include "kassert.h"
#include "kernel/typedefs.h"
#include "portable/irq.h"
#include "portable/scheduler.h"
#include "task.h"
#include "user/syscall.h"

typedef struct mutex_impl {
  task_t* owner;      // Pointer to the task that currently holds the lock
  task_t* wait_head;  // Pointer to the head of the waiting tasks list
  task_t* wait_tail;  // Pointer to the tail of the waiting tasks list
} mutex_impl_t;

typedef struct spinlock_impl {
  atomic_flag locked;
  task_t* owner;
} spinlock_impl_t;

typedef struct semaphore_impl {
  atomic_int value;
  atomic_int limit;
} semaphore_impl_t;

#define GET_MUTEX_IMPL(mtx) (mutex_impl_t*)(mtx)

extern task_t* current_task;
extern mutex_t tasks_mutex;

_Static_assert(sizeof(mutex_impl_t) <= sizeof(mutex_t),
               "mutex storage is too small for its implementation");
_Static_assert(_Alignof(mutex_t) >= _Alignof(mutex_impl_t),
               "mutex storage is not aligned for its implementation");
_Static_assert(sizeof(spinlock_impl_t) <= sizeof(spinlock_t),
               "spinlock storage is too small for its implementation");
_Static_assert(_Alignof(spinlock_t) >= _Alignof(spinlock_impl_t),
               "spinlock storage is not aligned for its implementation");
_Static_assert(sizeof(semaphore_impl_t) <= sizeof(semaphore_t),
               "semaphore storage is too small for its implementation");
_Static_assert(_Alignof(semaphore_t) >= _Alignof(semaphore_impl_t),
               "semaphore storage is not aligned for its implementation");

sync_result_t spinlock_lock(spinlock_t* lck) {
  spinlock_impl_t* impl = (spinlock_impl_t*)lck;
  while (
      atomic_flag_test_and_set_explicit(&impl->locked, memory_order_acquire));
  impl->owner = current_task;
  return S_SUCCESS;
}

sync_result_t spinlock_unlock(spinlock_t* lck) {
  spinlock_impl_t* impl = (spinlock_impl_t*)lck;
  impl->owner = 0;
  atomic_flag_clear_explicit(&impl->locked, memory_order_release);
  return S_SUCCESS;
}

sync_result_t Kernel_Mutex_Lock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  task_t* task = Get_Current_Task();
  mutex_impl_t* impl = GET_MUTEX_IMPL(mtx);

  if (impl->owner == task) {
    pIRQ_Restore(irq_state);
    return S_REENTER;
  }
  if (impl->owner == 0) {
    impl->owner = task;
    pIRQ_Restore(irq_state);
    return S_SUCCESS;
  }

  task->state = T_BLOCKED;
  task->wait_next = 0;

  if (impl->wait_tail) {
    impl->wait_tail->wait_next = task;
  } else {
    impl->wait_head = task;
  }
  impl->wait_tail = task;

  pIRQ_Restore(irq_state);
  // The task will be awakened by the Alarm_Task when it acquires the lock
  pPend_Task_Switch();
  // The task acquired the lock.
  return S_SUCCESS;
}

sync_result_t Kernel_Mutex_Unlock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  mutex_impl_t* impl = GET_MUTEX_IMPL(mtx);
  sync_result_t res;
  if (impl->owner == Get_Current_Task()) {
    if (impl->wait_head) {
      task_t* task = impl->wait_head;
      impl->wait_head = task->wait_next;
      task->wait_next = 0;
      if (impl->wait_tail == task) impl->wait_tail = 0;
      task->state = T_READY;
      impl->owner = task;
    } else {
      impl->owner = 0;
    }
    res = S_SUCCESS;
  } else {
    res = S_NOT_OWNER;
  }
  pIRQ_Restore(irq_state);
  return res;
}

sync_result_t Kernel_Mutex_Try_Lock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  // It may be set after mutex_unlock()
  mutex_impl_t* impl = GET_MUTEX_IMPL(mtx);
  if (impl->owner == Get_Current_Task()) {
    pIRQ_Restore(irq_state);
    return S_REENTER;
  }
  if (impl->owner == 0) {
    impl->owner = current_task;
    pIRQ_Restore(irq_state);
    return S_SUCCESS;
  }
  pIRQ_Restore(irq_state);
  return S_BUSY;
}

sync_result_t semaphore_init(semaphore_t* sem, int32_t limit) {
  semaphore_impl_t* impl = (semaphore_impl_t*)sem;
  atomic_store(&impl->value, limit);
  atomic_store(&impl->limit, limit);
  return S_SUCCESS;
}

sync_result_t semaphore_wait(semaphore_t* sem) {
  semaphore_impl_t* impl = (semaphore_impl_t*)sem;
  while (1) {
    uint32_t irq_state = pIRQ_Save();
    int32_t value = atomic_load(&impl->value);
    if (value > 0) {
      atomic_fetch_sub(&impl->value, 1);
      pIRQ_Restore(irq_state);
      return S_SUCCESS;
    }

    pIRQ_Restore(irq_state);
    Yield();
  }
}
sync_result_t semaphore_signal(semaphore_t* sem) {
  semaphore_impl_t* impl = (semaphore_impl_t*)sem;
  uint32_t irq_state = pIRQ_Save();
  int32_t value = atomic_load(&impl->value);
  int32_t limit = atomic_load(&impl->limit);
  if (value < limit) {
    atomic_store(&impl->value, value + 1);
  }
  pIRQ_Restore(irq_state);
  return S_SUCCESS;
}