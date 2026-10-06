#include "syscall.h"

#include "kassert.h"

void Syscall_Handler(uint32_t sysnum) {
  switch (sysnum) {
    case SYSCALL_TASK_SWITCH:
      pPend_Task_Switch();
      break;
    default:
      panic();
  }
}