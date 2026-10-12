#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>
#include <stdint.h>

typedef struct task task_t;

typedef struct spinlock {
  atomic_flag locked;
  task_t* task;
} spinlock_t;

void spinlock_lock(spinlock_t* lck);
void spinlock_unlock(spinlock_t* lck);

typedef spinlock_t mutex_t;

void mutex_lock(mutex_t* mtx);
void mutex_unlock(mutex_t* mtx);
// Returns 1 if the mutex was successfully locked, 0 otherwise
int mutex_try_lock(mutex_t* mtx);

typedef struct semaphore {
  atomic_int value;
  atomic_int limit;
} semaphore_t;

void semaphore_init(semaphore_t* sem, int32_t limit);
void semaphore_wait(semaphore_t* sem);
void semaphore_signal(semaphore_t* sem);

#endif