#include "sync.h"

#include <stdatomic.h>

#include "kassert.h"
#include "portable.h"
#include "syscall.h"
#include "task.h"

extern task_t* current_task;
extern mutex_t tasks_mutex;

void spinlock_lock(spinlock_t* lck) {
  while (atomic_flag_test_and_set_explicit(&lck->locked, memory_order_acquire));
  lck->owner = current_task;
}

void spinlock_unlock(spinlock_t* lck) {
  lck->owner = 0;
  atomic_flag_clear_explicit(&lck->locked, memory_order_release);
}

void mutex_lock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  task_t* task = Get_Current_Task();

  if (mtx->owner == 0 || mtx->owner == task) {
    mtx->owner = task;
    pIRQ_Restore(irq_state);
    return;
  }

  task->state = T_BLOCKED;
  task->wait_next = 0;

  if (mtx->wait_tail) {
    mtx->wait_tail->wait_next = task;
  } else {
    mtx->wait_head = task;
  }
  mtx->wait_tail = task;

  pIRQ_Restore(irq_state);
  Yield();
}

void mutex_unlock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  if (mtx->wait_head) {
    task_t* task = mtx->wait_head;
    mtx->wait_head = task->wait_next;
    task->wait_next = 0;
    if (mtx->wait_tail == task) mtx->wait_tail = 0;
    task->state = T_READY;
    mtx->owner = task;
  } else {
    mtx->owner = 0;
  }
  pIRQ_Restore(irq_state);
}

int mutex_try_lock(mutex_t* mtx) {
  uint32_t irq_state = pIRQ_Save();
  // It may be set after mutex_unlock()
  if (mtx->owner == Get_Current_Task()) return 1;
  if (mtx->owner == 0) {
    mtx->owner = current_task;
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