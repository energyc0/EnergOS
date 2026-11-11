#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "typedefs.h"

/*
  Synchronization primitives for single processor systems.
*/

#define MUTEX_INIT() {0}
#define SPINLOCK_INIT() {0}

sync_result_t spinlock_lock(spinlock_t* lck);
sync_result_t spinlock_unlock(spinlock_t* lck);

// Kernel-internal variants; use these instead of nesting a system call.
sync_result_t Kernel_Mutex_Lock(mutex_t* mtx);
sync_result_t Kernel_Mutex_Unlock(mutex_t* mtx);
sync_result_t Kernel_Mutex_Try_Lock(mutex_t* mtx);

sync_result_t semaphore_init(semaphore_t* sem, int32_t limit);
sync_result_t semaphore_wait(semaphore_t* sem);
sync_result_t semaphore_signal(semaphore_t* sem);

#endif