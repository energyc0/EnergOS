#include "syscall.h"

#include "kassert.h"
#include "portable.h"
#include "sync.h"
#include "task.h"

extern mutex_t tasks_mutex;
extern uint32_t task_count;
extern task_t* current_task;

void Syscall_Exit(void) {
  mutex_lock(&tasks_mutex);
  task_t* task = Get_Current_Task();
  Task_Delete(task);
  mutex_unlock(&tasks_mutex);
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