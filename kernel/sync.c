#include "sync.h"

#include "kassert.h"
#include "portable.h"
#include "task.h"

extern task_t* current_task;
static atomic_int irq_queries_number = 0;

static void IRQ_OFF_Numeric() {
  atomic_fetch_add(&irq_queries_number, 1);
  pDisable_IRQ();
}

static void IRQ_ON_Numeric() {
  int32_t num = atomic_load(&irq_queries_number);
  if (num > 0) {
    if (num == 1) pEnable_IRQ();
    atomic_fetch_sub(&irq_queries_number, 1);
  }
}

void spinlock_lock(spinlock_t* lck) {
  IRQ_OFF_Numeric();
  while (atomic_flag_test_and_set(&lck->locked));
  lck->task = current_task;
}
void spinlock_unlock(spinlock_t* lck) {
  lck->task = 0;
  atomic_flag_clear(&lck->locked);
  IRQ_ON_Numeric();
}

void mutex_lock(mutex_t* mtx) {
  while (1) {
    IRQ_OFF_Numeric();
    if (atomic_flag_test_and_set(&mtx->locked)) {
      IRQ_ON_Numeric();
      Task_Yield();
    } else {
      mtx->task = current_task;
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
  if (atomic_flag_test_and_set(&mtx->locked)) return 0;
  mtx->task = current_task;
  return 1;
}

void semaphore_wait(semaphore_t* sem) {
  while (1) {
    IRQ_OFF_Numeric();
    int32_t value = atomic_load(&sem->value);
    if (value > 0) {
      atomic_fetch_sub(&sem->value, 1);
      IRQ_ON_Numeric();
      return;
    }

    IRQ_ON_Numeric();
    Task_Yield();
  }
}
void semaphore_signal(semaphore_t* sem) {
  IRQ_OFF_Numeric();
  int32_t value = atomic_load(&sem->value);
  int32_t limit = atomic_load(&sem->limit);
  if (value < limit) {
    atomic_store(&sem->value, value + 1);
  }
  IRQ_ON_Numeric();
}