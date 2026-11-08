#include "syscall.h"

#include "port_syscall.h"
#include "syscall_defs.h"

void Jump_First_Task(void) { pSystem_Call(SYSCALL_JUMP_FIRST_TASK); }
void Yield(void) { pSystem_Call(SYSCALL_YIELD); }
void Exit(void) { pSystem_Call(SYSCALL_EXIT); }
uint32_t Create(const char* name, void (*entry)(void* param), void* param) {
  return pSystem_Call_Arg3(SYSCALL_CREATE, (uint32_t)name, (uint32_t)entry,
                           (uint32_t)param);
}
void Sleep(uint32_t ticks) { pSystem_Call_Arg1(SYSCALL_SLEEP, ticks); }