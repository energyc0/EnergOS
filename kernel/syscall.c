#include "syscall.h"

#include "kassert.h"
#include "portable.h"

void Syscall_Handler(uint32_t sysnum) {
  switch (sysnum) {
    case SYSCALL_JUMP_FIRST_TASK:
      pJump_First_Task();
      break;
    case SYSCALL_TASK_SWITCH:
      pPend_Task_Switch();
      break;
    default:
      panic();
  }
}