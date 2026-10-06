#include "task.h"

#include <stdint.h>

#include "kassert.h"
#include "kernel_config.h"
#include "portable.h"

enum task_state { T_UNUSED, T_READY, T_RUNNING };

// The first member of the struct task is the stack pointer,
// so we can cast a task_t* to a uint32_t* and get the stack pointer
typedef struct task {
  uint32_t *sp;
  void (*entry)(void);
  uint32_t stack[TASK_STACK_SIZE];
  uint32_t tid;
  const char *name;
  enum task_state state;
} task_t;

task_t tasks[NTASKS] = {};
uint32_t task_count = 0;
uint32_t task_start_id = 1;
task_t *current_task = 0;

void Init_Task(task_t *task, uint32_t tid, const char *name,
               void (*entry)(void)) {
  task->entry = entry;
  task->sp = pTask_Stack_Init(
      (uint32_t *)((uint8_t *)task->stack + sizeof(task->stack)), entry);
  task->name = name;
  task->tid = tid;
  task->state = T_READY;
}

uint32_t Task_Create(const char *name, void (*entry)(void)) {
  if (task_count >= NTASKS) return 0;

  task_t *task = &tasks[task_count++];
  Init_Task(task, task_start_id++, name, entry);
  return task->tid;
}

extern void Error_Handler(void);

void Scheduler_Switch(void) {
  if (current_task == 0) {
    if (task_count == 0) panic();
    current_task = &tasks[0];
    return;
  }

  uint32_t start = (current_task - tasks + 1) % task_count;

  for (uint32_t i = 0; i < task_count; i++) {
    uint32_t idx = (start + i) % task_count;
    if (tasks[idx].state == T_READY) {
      current_task->state = T_READY;
      current_task = &tasks[idx];
      current_task->state = T_RUNNING;
      return;
    }
  }
}
