#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>
#include <stdint.h>

/*
  Syncronization primitives for single processor systems.
*/

#define MUTEX_INIT() {0}
#define SPINLOCK_INIT() {0}

typedef struct task task_t;

typedef struct spinlock {
  atomic_flag locked;  // Flag indicating if the lock is held
  task_t* owner;       // Pointer to the task that currently holds the lock
} spinlock_t;

void spinlock_lock(spinlock_t* lck);
void spinlock_unlock(spinlock_t* lck);

typedef struct mutex {
  atomic_flag locked;  // Flag indicating if the lock is held
  task_t* owner;       // Pointer to the task that currently holds the lock
  task_t* wait_head;   // Pointer to the head of the waiting tasks list
  task_t* wait_tail;   // Pointer to the tail of the waiting tasks list
} mutex_t;

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