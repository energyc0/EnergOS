#include "sync.h"

#include <stdint.h>

#include "portable/syscall.h"
#include "kernel/syscall_defs.h"

void mutex_lock(mutex_t* mtx) {
  pSystem_Call_Arg1(SYSCALL_MUTEX_LOCK, (uint32_t)mtx);
}
void mutex_unlock(mutex_t* mtx) {
  pSystem_Call_Arg1(SYSCALL_MUTEX_UNLOCK, (uint32_t)mtx);
}
int mutex_try_lock(mutex_t* mtx) {
  return (int)pSystem_Call_Arg1(SYSCALL_MUTEX_TRY_LOCK, (uint32_t)mtx);
}