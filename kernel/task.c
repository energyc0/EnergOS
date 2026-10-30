#include "task.h"

#include <stdint.h>

#include "io.h"
#include "kassert.h"
#include "kernel_config.h"
#include "portable.h"
#include "sync.h"
#include "syscall.h"
#include "uart_queue.h"

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
    mutex_lock(&tasks_mutex);
    for (uint32_t i = 0; i < NTASKS; i++) {
      mutex_lock(&tasks[i].task_mutex);
      if (tasks[i].state == T_WAITING && tasks[i].wakeup_time < pGet_Tick()) {
        tasks[i].state = T_READY;
      }
      mutex_unlock(&tasks[i].task_mutex);
    }
    mutex_unlock(&tasks_mutex);
    Yield();
  }
}

void Print_Tasks(void *param) {
  (void)param;

  mutex_lock(&tasks_mutex);
  for (int i = 0; i < NTASKS; i++) {
    mutex_lock(&tasks[i].task_mutex);
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
    mutex_unlock(&tasks[i].task_mutex);
  }
  mutex_unlock(&tasks_mutex);
  Write_Char('\n');
}

static task_t *Find_Free_Task(void) {
  for (uint32_t i = 0; i < NTASKS; i++) {
    if (tasks[i].state == T_UNUSED) return &tasks[i];
  }
  return 0;
}

void Init_Task(task_t *task, uint32_t tid, const char *name,
               void (*entry)(void *param), void *param) {
  task->entry = entry;
  task->sp = pTask_Stack_Init(
      (uint32_t *)((uint8_t *)task->stack + sizeof(task->stack)), entry, param);
  task->name = name;
  task->tid = tid;
  task->state = T_READY;
  task->wait_next = 0;
  task->wakeup_time = 0;
}

uint32_t Task_Create(const char *name, void (*entry)(void *param),
                     void *param) {
  if (name == 0 || entry == 0) return 0;

  mutex_lock(&tasks_mutex);
  task_t *task = Find_Free_Task();
  if (task == 0) {
    mutex_unlock(&tasks_mutex);
    return 0;
  }

  mutex_lock(&task->task_mutex);
  Init_Task(task, task_start_id++, name, entry, param);
  task_count++;
  uint32_t ret = task->tid;
  mutex_unlock(&task->task_mutex);
  mutex_unlock(&tasks_mutex);
  return ret;
}

void Task_Delete(task_t *task) {
  mutex_lock(&tasks_mutex);
  if (task) {
    mutex_lock(&task->task_mutex);
    task->state = T_UNUSED;
    task_count--;
    mutex_unlock(&task->task_mutex);
  }
  mutex_unlock(&tasks_mutex);
}

void Scheduler_Switch(void) {
  // PendSV cannot wait on a mutex held by the task it just preempted.
  if (!mutex_try_lock(&tasks_mutex)) return;
  if (task_count == 0) {
    mutex_unlock(&tasks_mutex);
    return;
  }

  uint32_t start = current_task ? ((current_task - tasks + 1) % NTASKS) : 0;
  for (uint32_t i = 0; i < NTASKS; i++) {
    uint32_t idx = (start + i) % NTASKS;
    if (tasks[idx].state == T_UNUSED) continue;

    mutex_lock(&tasks[idx].task_mutex);
    if (tasks[idx].state == T_READY) {
      if (current_task && current_task->state == T_RUNNING)
        current_task->state = T_READY;
      current_task = &tasks[idx];
      current_task->state = T_RUNNING;
      mutex_unlock(&tasks[idx].task_mutex);
      mutex_unlock(&tasks_mutex);
      return;
    }
    mutex_unlock(&tasks[idx].task_mutex);
  }
  mutex_unlock(&tasks_mutex);
}

void Scheduler_Start(void) {
  Task_Create("Idle_Task", Idle_Task, 0);
  Task_Create("Alarm_Task", Alarm_Task, 0);
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