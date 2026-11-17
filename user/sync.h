#ifndef USER_SYNC_H
#define USER_SYNC_H

#include <stdint.h>

#include "kernel/typedefs.h"

#define MUTEX_INIT() {0}
#define SEMAPHORE_INIT(limit)                \
  {                                          \
    {                                        \
      (uintptr_t)(limit), (uintptr_t)(limit) \
    }                                        \
  }

sync_result_t Mutex_Lock(mutex_t* mtx);
sync_result_t Mutex_Unlock(mutex_t* mtx);
// Returns 1 if the mutex was successfully locked, 0 otherwise
sync_result_t Mutex_Try_Lock(mutex_t* mtx);

sync_result_t Semaphore_Wait(semaphore_t* sem);
sync_result_t Semaphore_Signal(semaphore_t* sem);

#endif