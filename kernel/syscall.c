#include "syscall.h"

#include <stdint.h>

#include "kassert.h"
#include "portable/irq.h"
#include "portable/scheduler.h"
#include "portable/syscall.h"
#include "portable/time.h"
#include "portable/uart.h"
#include "sync.h"
#include "syscall_defs.h"
#include "task.h"

extern mutex_t tasks_mutex;

static void Syscall_Exit(void) {
  Task_Delete(Get_Current_Task());
  pPend_Task_Switch();
}

static uint32_t Syscall_Create(const char *name, void (*entry)(void *param),
                               void *param) {
  return Task_Create(name, entry, param, T_USER);
}

static uint32_t Syscall_Wakeup(uint32_t tid) {
  if (tid == 0) return 0;
  Kernel_Mutex_Lock(&tasks_mutex);
  Kernel_Mutex_Unlock(&tasks_mutex);
  return tid;
}

static void Syscall_Sleep(uint32_t ticks) {
  if (ticks == 0) return;
  task_t *task = Get_Current_Task();
  Kernel_Mutex_Lock(&tasks_mutex);
  Kernel_Mutex_Lock(&task->task_mutex);
  task->state = T_WAITING;
  task->wakeup_time = pGet_Tick() + ticks;
  Kernel_Mutex_Unlock(&task->task_mutex);
  Kernel_Mutex_Unlock(&tasks_mutex);
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
      pSet_Current_Task_Privilege();
      pJump_First_Task();
      break;
    case SYSCALL_YIELD:
      pPend_Task_Switch();
      break;
    case SYSCALL_EXIT:
      Syscall_Exit();
      break;
    case SYSCALL_CREATE:
      *ret = (uint32_t)Syscall_Create((const char *)arg0,
                                      (void (*)(void *))arg1, (void *)arg2);
      break;
    case SYSCALL_SLEEP:
      Syscall_Sleep(arg0);
      break;
    case SYSCALL_WAKEUP:
      *ret = (uint32_t)Syscall_Wakeup(arg0);
      break;
    case SYSCALL_MUTEX_LOCK:
      *ret = (uint32_t)Kernel_Mutex_Lock((mutex_t *)arg0);
      break;
    case SYSCALL_MUTEX_UNLOCK:
      *ret = (uint32_t)Kernel_Mutex_Unlock((mutex_t *)arg0);
      break;
    case SYSCALL_MUTEX_TRY_LOCK:
      *ret = (uint32_t)Kernel_Mutex_Try_Lock((mutex_t *)arg0);
      break;
    case SYSCALL_PRINT_TASKS:
      Kernel_Print_Tasks();
      break;
    case SYSCALL_UART_READ:
      *ret = pUART_ReceiveByte((uint8_t *)arg0, 0);
      break;
    default:
      panic("Undefined syscall number");
  }
}