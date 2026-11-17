#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "typedefs.h"

/*
  Synchronization primitives for single processor systems.
*/

#define MUTEX_INIT() {0}
#define SPINLOCK_INIT() {0}
#define SEMAPHORE_INIT(limit)                \
  {                                          \
    {                                        \
      (uintptr_t)(limit), (uintptr_t)(limit) \
    }                                        \
  }

sync_result_t Spinlock_Lock(spinlock_t* lck);
sync_result_t Spinlock_Unlock(spinlock_t* lck);

// Kernel-internal variants; use these instead of nesting a system call.
sync_result_t Kernel_Mutex_Lock(mutex_t* mtx);
sync_result_t Kernel_Mutex_Unlock(mutex_t* mtx);
sync_result_t Kernel_Mutex_Try_Lock(mutex_t* mtx);

sync_result_t Kernel_Semaphore_Wait(semaphore_t* sem);
sync_result_t Kernel_Semaphore_Signal(semaphore_t* sem);

#endif