#include "syscall.h"

#include "kernel/syscall_defs.h"
#include "portable/syscall.h"

void Jump_First_Task(void) { pSystem_Call(SYSCALL_JUMP_FIRST_TASK); }
void Yield(void) { pSystem_Call(SYSCALL_YIELD); }
void Exit(void) { pSystem_Call(SYSCALL_EXIT); }
uint32_t Create(const char* name, void (*entry)(void* param), void* param) {
  return pSystem_Call_Arg3(SYSCALL_CREATE, (uint32_t)name, (uint32_t)entry,
                           (uint32_t)param);
}
void Print_Task_List(void) { pSystem_Call(SYSCALL_PRINT_TASKS); }
void Sleep(uint32_t ticks) { pSystem_Call_Arg1(SYSCALL_SLEEP, ticks); }
uint32_t Wait(uint32_t tid) {
  for (;;) {
    uint32_t result = pSystem_Call_Arg1(SYSCALL_WAIT, tid);
    if (result == WAIT_RESULT_COMPLETE) return tid;
    if (result == WAIT_RESULT_INVALID) return 0;
    // If WAIT_RESULT_PENDING then try again
  }
}