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

sync_result_t mutex_lock(mutex_t* mtx);
sync_result_t mutex_unlock(mutex_t* mtx);
// Returns 1 if the mutex was successfully locked, 0 otherwise
sync_result_t mutex_try_lock(mutex_t* mtx);

sync_result_t semaphore_wait(semaphore_t* sem);
sync_result_t semaphore_signal(semaphore_t* sem);

#endif