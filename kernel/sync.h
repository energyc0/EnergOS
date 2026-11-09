#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "typedefs.h"

/*
  Syncronization primitives for single processor systems.
*/

#define MUTEX_INIT() {0}
#define SPINLOCK_INIT() {0}

void spinlock_lock(spinlock_t* lck);
void spinlock_unlock(spinlock_t* lck);

// Kernel-internal variants; use these instead of nesting a system call.
void Kernel_Mutex_Lock(mutex_t* mtx);
void Kernel_Mutex_Unlock(mutex_t* mtx);
int Kernel_Mutex_Try_Lock(mutex_t* mtx);

void semaphore_init(semaphore_t* sem, int32_t limit);
void semaphore_wait(semaphore_t* sem);
void semaphore_signal(semaphore_t* sem);

#endif