#include "sync.h"

#include <stdatomic.h>

#include "kassert.h"
#include "portable.h"
#include "syscall.h"
#include "task.h"

extern task_t* current_task;

void spinlock_lock(spinlock_t* lck) {
  while (atomic_flag_test_and_set_explicit(&lck->locked, memory_order_acquire));
  lck->task = current_task;
}
void spinlock_unlock(spinlock_t* lck) {
  lck->task = 0;
  atomic_flag_clear_explicit(&lck->locked, memory_order_release);
}

void mutex_lock(mutex_t* mtx) {
  while (1) {
    uint32_t irq_state = pIRQ_Save();
    if (atomic_flag_test_and_set_explicit(&mtx->locked, memory_order_acquire)) {
      pIRQ_Restore(irq_state);
      Yield();
    } else {
      mtx->task = current_task;
      pIRQ_Restore(irq_state);
      break;
    }
  }
}
void mutex_unlock(mutex_t* mtx) { spinlock_unlock(mtx); }

void semaphore_init(semaphore_t* sem, int32_t limit) {
  atomic_store(&sem->value, limit);
  atomic_store(&sem->limit, limit);
}

int mutex_try_lock(mutex_t* mtx) {
  if (atomic_flag_test_and_set_explicit(&mtx->locked, memory_order_acquire))
    return 0;
  mtx->task = current_task;
  return 1;
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