#include "sync.h"

#include <stdatomic.h>

#include "kassert.h"
#include "kernel/typedefs.h"
#include "portable/irq.h"
#include "portable/scheduler.h"
#include "task.h"

typedef struct mutex_impl {
  task_t* owner;
  event_t event;
} mutex_impl_t;

typedef struct spinlock_impl {
  atomic_flag locked;
  task_t* owner;
} spinlock_impl_t;

typedef struct semaphore_impl {
  atomic_int value;
  atomic_int limit;
  event_t event;
} semaphore_impl_t;

typedef struct event_impl {
  uint32_t flags;
  list_t* wait_head;
  list_t* wait_tail;
} event_impl_t;

#define GET_MUTEX_IMPL(mtx) (mutex_impl_t*)(mtx)
#define GET_EVENT_IMPL(event) ((event_impl_t*)(event))
#define EVENT_AVAILABLE (1u << 0)

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
_Static_assert(sizeof(event_impl_t) <= sizeof(event_t),
               "event storage is too small for its implementation");
_Static_assert(_Alignof(event_t) >= _Alignof(event_impl_t),
               "event storage is not aligned for its implementation");

static void Event_Enqueue(event_impl_t* event, task_t* task,
                          uint32_t wait_mask) {
  task->wait_node.value = task;
  task->wait_node.next = 0;
  task->wait_mask = wait_mask;

  if (event->wait_tail) {
    event->wait_tail->next = &task->wait_node;
  } else {
    event->wait_head = &task->wait_node;
  }
  event->wait_tail = &task->wait_node;
}

static task_t* Event_Signal(event_impl_t* event, uint32_t flags) {
  event->flags |= flags;

  list_t* previous = 0;
  list_t* node = event->wait_head;
  while (node) {
    task_t* task = node->value;
    uint32_t matched_flags = task->wait_mask & event->flags;
    if (matched_flags) {
      if (previous) {
        previous->next = node->next;
      } else {
        event->wait_head = node->next;
      }
      if (event->wait_tail == node) event->wait_tail = previous;

      event->flags &= ~matched_flags;
      task->wait_node.value = 0;
      task->wait_node.next = 0;
      task->wait_mask = 0;
      task->state = T_READY;
      return task;
    }

    previous = node;
    node = node->next;
  }
  return 0;
}

sync_result_t Spinlock_Lock(spinlock_t* lck) {
  spinlock_impl_t* impl = (spinlock_impl_t*)lck;
  while (
      atomic_flag_test_and_set_explicit(&impl->locked, memory_order_acquire));
  impl->owner = current_task;
  return S_SUCCESS;
}

sync_result_t Spinlock_Unlock(spinlock_t* lck) {
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
    GET_EVENT_IMPL(&impl->event)->flags &= ~EVENT_AVAILABLE;
    pIRQ_Restore(irq_state);
    return S_SUCCESS;
  }

  task->state = T_BLOCKED;
  Event_Enqueue(GET_EVENT_IMPL(&impl->event), task, EVENT_AVAILABLE);
  pIRQ_Restore(irq_state);
  pPend_Task_Switch();
  return S_SUCCESS;
}

sync_result_t Kernel_Mutex_Unlock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  mutex_impl_t* impl = GET_MUTEX_IMPL(mtx);
  sync_result_t res;
  if (impl->owner == Get_Current_Task()) {
    event_impl_t* event = GET_EVENT_IMPL(&impl->event);
    task_t* task = Event_Signal(event, EVENT_AVAILABLE);
    if (task) {
      impl->owner = task;
    } else {
      impl->owner = 0;
      event->flags |= EVENT_AVAILABLE;
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
    GET_EVENT_IMPL(&impl->event)->flags &= ~EVENT_AVAILABLE;
    pIRQ_Restore(irq_state);
    return S_SUCCESS;
  }
  pIRQ_Restore(irq_state);
  return S_BUSY;
}

sync_result_t Kernel_Semaphore_Wait(semaphore_t* sem) {
  semaphore_impl_t* impl = (semaphore_impl_t*)sem;
  uint32_t irq_state = pIRQ_Save();
  task_t* task = Get_Current_Task();
  int32_t value = atomic_load(&impl->value);
  if (value > 0) {
    atomic_fetch_sub(&impl->value, 1);
    if (value == 1) GET_EVENT_IMPL(&impl->event)->flags &= ~EVENT_AVAILABLE;
    pIRQ_Restore(irq_state);
    return S_SUCCESS;
  }

  task->state = T_BLOCKED;
  Event_Enqueue(GET_EVENT_IMPL(&impl->event), task, EVENT_AVAILABLE);
  pIRQ_Restore(irq_state);
  pPend_Task_Switch();
  return S_SUCCESS;
}
sync_result_t Kernel_Semaphore_Signal(semaphore_t* sem) {
  semaphore_impl_t* impl = (semaphore_impl_t*)sem;
  uint32_t irq_state = pIRQ_Save();
  int32_t value = atomic_load(&impl->value);
  int32_t limit = atomic_load(&impl->limit);
  if (value < limit &&
      !Event_Signal(GET_EVENT_IMPL(&impl->event), EVENT_AVAILABLE)) {
    atomic_store(&impl->value, value + 1);
  }
  pIRQ_Restore(irq_state);
  return S_SUCCESS;
}