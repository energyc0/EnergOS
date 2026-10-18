#include "syscall.h"

#include "kassert.h"
#include "portable.h"
#include "task.h"

void Syscall_Exit(void) {
  Task_Delete(Get_Current_Task());
  pPend_Task_Switch();
}

void Syscall_Create(void) {
  /*
  mutex_lock(&tasks_mutex);
  task_t* task = Get_Current_Task();
  task->state = T_UNUSED;
  mutex_unlock(&task->task_mutex);
  mutex_unlock(&tasks_mutex);
  */
}

void Syscall_Handler(uint32_t sysnum) {
  switch (sysnum) {
    case SYSCALL_JUMP_FIRST_TASK:
      pJump_First_Task();
      break;
    case SYSCALL_YIELD:
      pPend_Task_Switch();
      break;
    case SYSCALL_EXIT:
      Syscall_Exit();
      break;
    case SYSCALL_CREATE:
      Syscall_Create();
      break;
    default:
      panic("Undefined syscall number");
  }
}