#include "task.h"

#include <stdint.h>

#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "syscall.h"

#define TASK_STACK_SIZE 256
#define NTASKS 16

enum task_state { T_UNUSED, T_READY, T_RUNNING };

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
task_t *current_task = NULL;

uint32_t *Task_Stack_Init(uint32_t *stack_top, void (*task_entry)(void)) {
  // Create exception frame
  stack_top -= 8;
  stack_top[7] = 0x1000000;             // xPSR, enable Thumb mode, 24 bit = 1
  stack_top[6] = (uint32_t)task_entry;  // Return address
  stack_top[5] =
      0xfffffffd;    // Switch to Unprivileged Handler Mode, unstack next task’s
                     // exception frame and continue on its PC.
  stack_top[4] = 0;  // R12
  stack_top[3] = 0;  // R3
  stack_top[2] = 0;  // R2
  stack_top[1] = 0;  // R1
  stack_top[0] = 0;  // R0

  *(--stack_top) = 0xFFFFFFFD;  // EXC_RETURN
  stack_top -= 8;               // Reserve space for R4-R11

  return stack_top;
}

void Init_Task(task_t *task, uint32_t tid, const char *name,
               void (*entry)(void)) {
  task->entry = entry;
  task->sp = Task_Stack_Init(
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

void Task_Yield() { __asm volatile("SVC %0" : : "n"(SYSCALL_TASK_SWITCH)); }

void Scheduler_Init(void) {
  // No preemtion, only subpriority
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
  // Set the lowest priority for interrupts
  HAL_NVIC_SetPriority(PendSV_IRQn, 0b1111, 0);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0b1111, 0);
  HAL_NVIC_EnableIRQ(PendSV_IRQn);
  HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

extern void Error_Handler(void);

void Scheduler_Switch(void) {
  if (current_task == NULL) {
    if (task_count == 0) Error_Handler();
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

void Scheduler_Start(void) {
  Scheduler_Switch();
  __set_PSP((uint32_t)current_task->sp);
  // Unprivileged Handler Mode
  __set_CONTROL(0x3);
  __ISB();

  current_task->entry();
  while (1);
}