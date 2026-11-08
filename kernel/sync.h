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

/* Opaque storage for statically allocated mutexes. */
typedef struct mutex {
  uintptr_t _opaque[3];
} mutex_t;

// Kernel-internal variants; use these instead of nesting a system call.
void Kernel_Mutex_Lock(mutex_t* mtx);
void Kernel_Mutex_Unlock(mutex_t* mtx);
int Kernel_Mutex_Try_Lock(mutex_t* mtx);

typedef struct semaphore {
  atomic_int value;
  atomic_int limit;
} semaphore_t;

void semaphore_init(semaphore_t* sem, int32_t limit);
void semaphore_wait(semaphore_t* sem);
void semaphore_signal(semaphore_t* sem);

#endif