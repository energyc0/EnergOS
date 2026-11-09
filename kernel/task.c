#include "task.h"

#include <stdint.h>

#include "kassert.h"
#include "portable/kernel_config.h"
#include "portable/scheduler.h"
#include "portable/time.h"
#include "sync.h"
#include "syscall.h"
#include "uart_queue.h"
#include "user/io.h"

// Mutex to access tasks[], task_count, current_task and task_start_id
mutex_t tasks_mutex = MUTEX_INIT();
// Need to lock tasks_mutex before using
task_t tasks[NTASKS] = {};
// Need to lock tasks_mutex before using
uint32_t task_count = 0;
// Need to lock tasks_mutex before using
uint32_t task_start_id = 1;
// Need to lock tasks_mutex before using
task_t *current_task = 0;

static void Idle_Task(void *param) {
  (void)param;
  while (1);
}

static void Alarm_Task(void *param) {
  (void)param;
  while (1) {
    Kernel_Mutex_Lock(&tasks_mutex);
    for (uint32_t i = 0; i < NTASKS; i++) {
      Kernel_Mutex_Lock(&tasks[i].task_mutex);
      if (tasks[i].state == T_WAITING && tasks[i].wakeup_time < pGet_Tick()) {
        tasks[i].state = T_READY;
      }
      Kernel_Mutex_Unlock(&tasks[i].task_mutex);
    }
    Kernel_Mutex_Unlock(&tasks_mutex);
    Yield();
  }
}

void Print_Tasks(void *param) {
  (void)param;

  Kernel_Mutex_Lock(&tasks_mutex);
  for (int i = 0; i < NTASKS; i++) {
    Kernel_Mutex_Lock(&tasks[i].task_mutex);
    if (tasks[i].state != T_UNUSED) {
      Write_Str("Task name: ");
      Write_Str(tasks[i].name);
      Write_Str(" ,TID: ");
      Write_Int(tasks[i].tid);
      Write_Str(" , state: ");
      switch (tasks[i].state) {
        case T_READY:
          Write_Str("READY");
          break;
        case T_RUNNING:
          Write_Str("RUNNING");
          break;
        case T_WAITING:
          Write_Str("WAITING");
          break;
        case T_BLOCKED:
          Write_Str("BLOCKED");
          break;
        default:
          panic("Undefined task state");
      }
    }
    Kernel_Mutex_Unlock(&tasks[i].task_mutex);
  }
  Kernel_Mutex_Unlock(&tasks_mutex);
  Write_Char('\n');
}

static task_t *Find_Free_Task(void) {
  for (uint32_t i = 0; i < NTASKS; i++) {
    if (tasks[i].state == T_UNUSED) return &tasks[i];
  }
  return 0;
}

void Init_Task(task_t *task, uint32_t tid, const char *name,
               void (*entry)(void *param), void *param,
               enum task_privilege privilege) {
  task->entry = entry;
  task->sp = pTask_Stack_Init(
      (uint32_t *)((uint8_t *)task->stack + sizeof(task->stack)), entry, param);
  task->name = name;
  task->tid = tid;
  task->state = T_READY;
  task->privilege = privilege;
  task->wait_next = 0;
  task->wakeup_time = 0;
}

uint32_t Task_Create(const char *name, void (*entry)(void *param), void *param,
                     enum task_privilege privilege) {
  if (name == 0 || entry == 0 || (privilege != T_USER && privilege != T_KERNEL))
    return 0;

  Kernel_Mutex_Lock(&tasks_mutex);
  task_t *task = Find_Free_Task();
  if (task == 0) {
    Kernel_Mutex_Unlock(&tasks_mutex);
    return 0;
  }

  Kernel_Mutex_Lock(&task->task_mutex);
  Init_Task(task, task_start_id++, name, entry, param, privilege);
  task_count++;
  uint32_t ret = task->tid;
  Kernel_Mutex_Unlock(&task->task_mutex);
  Kernel_Mutex_Unlock(&tasks_mutex);
  return ret;
}

void Task_Delete(task_t *task) {
  Kernel_Mutex_Lock(&tasks_mutex);
  if (task) {
    Kernel_Mutex_Lock(&task->task_mutex);
    task->state = T_UNUSED;
    task_count--;
    Kernel_Mutex_Unlock(&task->task_mutex);
  }
  Kernel_Mutex_Unlock(&tasks_mutex);
}

void Scheduler_Switch(void) {
  // PendSV cannot wait on a mutex held by the task it just preempted.
  if (!Kernel_Mutex_Try_Lock(&tasks_mutex)) return;
  if (task_count == 0) {
    Kernel_Mutex_Unlock(&tasks_mutex);
    return;
  }

  uint32_t start = current_task ? ((current_task - tasks + 1) % NTASKS) : 0;
  for (uint32_t i = 0; i < NTASKS; i++) {
    uint32_t idx = (start + i) % NTASKS;
    if (tasks[idx].state == T_UNUSED) continue;

    Kernel_Mutex_Lock(&tasks[idx].task_mutex);
    if (tasks[idx].state == T_READY) {
      if (current_task && current_task->state == T_RUNNING)
        current_task->state = T_READY;
      current_task = &tasks[idx];
      current_task->state = T_RUNNING;
      Kernel_Mutex_Unlock(&tasks[idx].task_mutex);
      Kernel_Mutex_Unlock(&tasks_mutex);
      return;
    }
    Kernel_Mutex_Unlock(&tasks[idx].task_mutex);
  }
  Kernel_Mutex_Unlock(&tasks_mutex);
}

void Scheduler_Start(void) {
  Task_Create("Idle_Task", Idle_Task, 0, T_KERNEL);
  Task_Create("Alarm_Task", Alarm_Task, 0, T_KERNEL);
  Uart_Queue_Init();
  pDisable_IRQ();
  pScheduler_Init();
  current_task = &tasks[0];
  current_task->state = T_RUNNING;
  pEnable_IRQ();
  // Pick the first task
  pStart_First_Task();
}

task_t *Get_Current_Task(void) { return current_task; }