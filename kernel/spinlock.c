#include "spinlock.h"

#include "task.h"

extern task_t* current_task;

void lock(spinlock_t* lck) {
  while (atomic_flag_test_and_set(&lck->locked));
  lck->task = current_task;
}
void unlock(spinlock_t* lck) {
  lck->task = 0;
  atomic_flag_clear(&lck->locked);
}