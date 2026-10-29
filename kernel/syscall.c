#include "syscall.h"

#include <stdint.h>

#include "kassert.h"
#include "portable.h"
#include "sync.h"
#include "syscall_defs.h"
#include "task.h"

extern mutex_t tasks_mutex;

void Jump_First_Task(void) { pSystem_Call(SYSCALL_JUMP_FIRST_TASK); }
void Yield(void) { pSystem_Call(SYSCALL_YIELD); }
void Exit(void) { pSystem_Call(SYSCALL_EXIT); }
uint32_t Create(const char *name, void (*entry)(int argc, char **argv)) {
  return pSystem_Call_Arg2(SYSCALL_CREATE, (uint32_t)(uintptr_t)name,
                           (uint32_t)(uintptr_t)entry);
}
void Sleep(uint32_t ticks) { pSystem_Call_Arg1(SYSCALL_SLEEP, ticks); }

static void Syscall_Exit(void) {
  Task_Delete(Get_Current_Task());
  pPend_Task_Switch();
}

static uint32_t Syscall_Create(const char *name,
                               void (*entry)(int argc, char **argv)) {
  return Task_Create(name, entry);
}

static uint32_t Syscall_Wakeup(uint32_t tid) {
  if (tid == 0) return 0;
  mutex_lock(&tasks_mutex);
  mutex_unlock(&tasks_mutex);
  return tid;
}

static void Syscall_Sleep(uint32_t ticks) {
  if (ticks == 0) return;
  task_t *task = Get_Current_Task();
  mutex_lock(&tasks_mutex);
  mutex_lock(&task->task_mutex);
  task->state = T_WAITING;
  task->wakeup_time = pGet_Tick() + ticks;
  mutex_unlock(&task->task_mutex);
  mutex_unlock(&tasks_mutex);
  pPend_Task_Switch();
}
/*
void Pause() {
  task_t *task = Get_Current_Task();
  mutex_lock(&task->task_mutex);
  task->state =
  mutex_unlock(&task->task_mutex);
  Yield();
}
*/

void Syscall_Handler(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                     uint32_t arg2, uint32_t arg3, uint32_t *ret) {
  (void)arg2;
  (void)arg3;
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
      *ret = Syscall_Create((const char *)(uintptr_t)arg0,
                            (void (*)(int argc, char **argv))(uintptr_t)arg1);
      break;
    case SYSCALL_SLEEP:
      Syscall_Sleep(arg0);
      break;
    case SYSCALL_WAKEUP:
      *ret = Syscall_Wakeup(arg0);
      break;
    default:
      panic("Undefined syscall number");
  }
}