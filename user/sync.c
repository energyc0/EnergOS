#include "sync.h"

#include <stdint.h>

#include "kernel/syscall_defs.h"
#include "portable/syscall.h"

sync_result_t mutex_lock(mutex_t* mtx) {
  return pSystem_Call_Arg1(SYSCALL_MUTEX_LOCK, (uint32_t)mtx);
}
sync_result_t mutex_unlock(mutex_t* mtx) {
  return pSystem_Call_Arg1(SYSCALL_MUTEX_UNLOCK, (uint32_t)mtx);
}
sync_result_t mutex_try_lock(mutex_t* mtx) {
  return (int)pSystem_Call_Arg1(SYSCALL_MUTEX_TRY_LOCK, (uint32_t)mtx);
}

sync_result_t semaphore_wait(semaphore_t* sem) {
  return pSystem_Call_Arg1(SYSCALL_SEMAPHORE_WAIT, (uint32_t)sem);
}

sync_result_t semaphore_signal(semaphore_t* sem) {
  return pSystem_Call_Arg1(SYSCALL_SEMAPHORE_SIGNAL, (uint32_t)sem);
}