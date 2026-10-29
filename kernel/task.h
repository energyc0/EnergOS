#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#include "kernel_config.h"
#include "sync.h"
/*
  T_UNUSED - task is not initialized.
  T_READY - task is ready to execute.
  T_RUNNING - this is the current task.
  T_WAITING - task is sleeping and can be awakened by Wakeup().
  T_BLOCKED - task is blocked by a sync primitive.
*/
enum task_state { T_UNUSED, T_READY, T_RUNNING, T_WAITING, T_BLOCKED };

// The first member of the struct task is the stack pointer,
// so we can cast a task_t* to a uint32_t* and get the stack pointer
typedef struct task {
  uint32_t *sp;
  void (*entry)(int argc, char **argv);
  uint32_t stack[TASK_STACK_SIZE];
  uint32_t tid;
  const char *name;
  enum task_state state;
  mutex_t task_mutex;      // Mutex to access task data
  uint32_t wakeup_time;    // Time when the task should wake up, in milliseconds
  struct task *wait_next;  // Pointer to next task in the wait list (for
                           // synchronization primitives)
} task_t;

// Creates task and returns its id
// Valid tid is a value more than 0
// Return 0 on error
uint32_t Task_Create(const char *name, void (*entry)(int argc, char **argv));

void Task_Delete(task_t *task);

// Sleep until somebody calls wakeup
void Task_Sleep();

void Scheduler_Start(void);

task_t *Get_Current_Task(void);

#endif