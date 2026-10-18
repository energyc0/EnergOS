#include "task.h"

#include <stdint.h>

#include "kassert.h"
#include "portable.h"
#include "sync.h"
#include "syscall.h"

// Mutex to access tasks[], task_count, current_task and task_start_id
mutex_t tasks_mutex;
// Need to lock tasks_mutex before using
task_t tasks[NTASKS] = {};
// Need to lock tasks_mutex before using
uint32_t task_count = 0;
// Need to lock tasks_mutex before using
uint32_t task_start_id = 1;
// Need to lock tasks_mutex before using
task_t *current_task = 0;

static void Idle_Task(int argc, char **argv) {
  (void)argc;
  (void)argv;
  while (1);
}

#include "io.h"
void Print_Tasks(int argc, char **argv) {
  (void)argc;
  (void)argv;
  mutex_lock(&tasks_mutex);
  for (int i = 0; i < NTASKS; i++) {
    mutex_lock(&tasks[i].task_mutex);
    if (tasks[i].state != T_UNUSED) {
      Write_Str("Task name: ");
      Write_Str(tasks[i].name);
      Write_Str(" ,TID: ");
      Write_Int(tasks[i].tid);
      Write_Str(" , state: ");
      const char *state_str = 0;
      switch (tasks[i].state) {
        case T_READY:
          state_str = "READY";
          break;
        case T_RUNNING:
          state_str = "RUNNING";
          break;
        case T_WAITING:
          state_str = "WAITING";
          break;
        default:
          panic("Undefined task state");
      }
      Write_Str(state_str);
      Write_Char('\n');
    }
    mutex_unlock(&tasks[i].task_mutex);
  }
  mutex_unlock(&tasks_mutex);
}

static task_t *Find_Free_Task(void) {
  for (uint32_t i = 0; i < NTASKS; i++) {
    if (tasks[i].state == T_UNUSED) return &tasks[i];
  }
  return 0;
}

void Init_Task(task_t *task, uint32_t tid, const char *name,
               void (*entry)(int argc, char **argv)) {
  task->entry = entry;
  task->sp = pTask_Stack_Init(
      (uint32_t *)((uint8_t *)task->stack + sizeof(task->stack)), entry);
  task->name = name;
  task->tid = tid;
  task->state = T_READY;
}

uint32_t Task_Create(const char *name, void (*entry)(int argc, char **argv)) {
  if (name == 0 || entry == 0) return 0;

  mutex_lock(&tasks_mutex);
  task_t *task = Find_Free_Task();
  if (task == 0) {
    mutex_unlock(&tasks_mutex);
    return 0;
  }

  mutex_lock(&task->task_mutex);
  Init_Task(task, task_start_id++, name, entry);
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
      if (current_task && current_task->state != T_UNUSED)
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
  Task_Create("Idle_Task", Idle_Task);
  pDisable_IRQ();
  pScheduler_Init();
  current_task = &tasks[0];
  current_task->state = T_RUNNING;
  pEnable_IRQ();
  // Pick the first task
  pStart_First_Task();
}

task_t *Get_Current_Task(void) { return current_task; }