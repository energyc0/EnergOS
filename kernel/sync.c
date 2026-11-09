#include "sync.h"

#include <stdatomic.h>

#include "kassert.h"
#include "portable/irq.h"
#include "portable/scheduler.h"
#include "syscall.h"
#include "task.h"

typedef struct mutex_impl {
  task_t* owner;      // Pointer to the task that currently holds the lock
  task_t* wait_head;  // Pointer to the head of the waiting tasks list
  task_t* wait_tail;  // Pointer to the tail of the waiting tasks list
} mutex_impl_t;

extern task_t* current_task;
extern mutex_t tasks_mutex;

_Static_assert(sizeof(mutex_impl_t) <= sizeof(mutex_t),
               "mutex storage is too small for its implementation");
_Static_assert(_Alignof(mutex_t) >= _Alignof(mutex_impl_t),
               "mutex storage is not aligned for its implementation");

static void Mutex_Load(const mutex_t* mtx, mutex_impl_t* impl) {
  const uint8_t* src = (const uint8_t*)mtx;
  uint8_t* dst = (uint8_t*)impl;
  for (uint32_t i = 0; i < sizeof(*impl); i++) dst[i] = src[i];
}

static void Mutex_Store(mutex_t* mtx, const mutex_impl_t* impl) {
  uint8_t* dst = (uint8_t*)mtx;
  const uint8_t* src = (const uint8_t*)impl;
  for (uint32_t i = 0; i < sizeof(*impl); i++) dst[i] = src[i];
}

void spinlock_lock(spinlock_t* lck) {
  while (atomic_flag_test_and_set_explicit(&lck->locked, memory_order_acquire));
  lck->owner = current_task;
}

void spinlock_unlock(spinlock_t* lck) {
  lck->owner = 0;
  atomic_flag_clear_explicit(&lck->locked, memory_order_release);
}

void Kernel_Mutex_Lock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  task_t* task = Get_Current_Task();
  mutex_impl_t impl;
  Mutex_Load(mtx, &impl);

  if (impl.owner == 0 || impl.owner == task) {
    impl.owner = task;
    Mutex_Store(mtx, &impl);
    pIRQ_Restore(irq_state);
    return;
  }

  task->state = T_BLOCKED;
  task->wait_next = 0;

  if (impl.wait_tail) {
    impl.wait_tail->wait_next = task;
  } else {
    impl.wait_head = task;
  }
  impl.wait_tail = task;
  Mutex_Store(mtx, &impl);

  pIRQ_Restore(irq_state);
  pPend_Task_Switch();
}

void Kernel_Mutex_Unlock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  mutex_impl_t impl;
  Mutex_Load(mtx, &impl);
  if (impl.wait_head) {
    task_t* task = impl.wait_head;
    impl.wait_head = task->wait_next;
    task->wait_next = 0;
    if (impl.wait_tail == task) impl.wait_tail = 0;
    task->state = T_READY;
    impl.owner = task;
  } else {
    impl.owner = 0;
  }
  Mutex_Store(mtx, &impl);
  pIRQ_Restore(irq_state);
}

int Kernel_Mutex_Try_Lock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  // It may be set after mutex_unlock()
  mutex_impl_t impl;
  Mutex_Load(mtx, &impl);
  if (impl.owner == Get_Current_Task()) {
    pIRQ_Restore(irq_state);
    return 1;
  }
  if (impl.owner == 0) {
    impl.owner = current_task;
    Mutex_Store(mtx, &impl);
    pIRQ_Restore(irq_state);
    return 1;
  } else {
    pIRQ_Restore(irq_state);
    return 0;
  }
}

void semaphore_init(semaphore_t* sem, int32_t limit) {
  atomic_store(&sem->value, limit);
  atomic_store(&sem->limit, limit);
}

void semaphore_wait(semaphore_t* sem) {
  while (1) {
    uint32_t irq_state = pIRQ_Save();
    int32_t value = atomic_load(&sem->value);
    if (value > 0) {
      atomic_fetch_sub(&sem->value, 1);
      pIRQ_Restore(irq_state);
      return;
    }

    pIRQ_Restore(irq_state);
    Yield();
  }
}
void semaphore_signal(semaphore_t* sem) {
  uint32_t irq_state = pIRQ_Save();
  int32_t value = atomic_load(&sem->value);
  int32_t limit = atomic_load(&sem->limit);
  if (value < limit) {
    atomic_store(&sem->value, value + 1);
  }
  pIRQ_Restore(irq_state);
}