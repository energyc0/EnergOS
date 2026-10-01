#include "task.h"

#include <stdint.h>

#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"

#define TASK_STACK_SIZE 256
#define NTASKS 16

typedef struct task {
  uint32_t tid;
  const char *name;
  uint32_t stack[TASK_STACK_SIZE];
  uint32_t *sp;
  void (*entry)(void);
} task_t;

task_t tasks[NTASKS];
uint32_t task_count = 0;
uint32_t task_start_id = 1;

uint32_t *Task_Stack_Init(uint32_t *stack_top, void (*task_entry)(void)) {
  stack_top -= 9;                       // Reserve space for R4-R11
  stack_top[8] = (uint32_t)task_entry;  // LR (EXC_RETURN)
  // stack_top -= 8;
  // stack_top[7] = 0x01000000;            // xPSR (Thumb bit = 1, thread mode)
  // stack_top[6] = (uint32_t)task_entry;  // PC
  // stack_top[5] = 0xFFFFFFFD;            // LR (EXC_RETURN)
  //  R12, R3-R0 registers stack_top[4..0]

  return stack_top;
}

void Init_Task(task_t *task, uint32_t tid, const char *name,
               void (*entry)(void)) {
  task->entry = entry;
  task->sp = Task_Stack_Init(
      (uint32_t *)((uint8_t *)task->stack + sizeof(task->stack)), entry);
  task->name = name;
  task->tid = tid;
}

uint32_t Task_Create(const char *name, void (*entry)(void)) {
  if (task_count >= NTASKS) return 0;

  task_t *task = &tasks[task_count++];
  Init_Task(task, task_start_id++, name, entry);
  return task->tid;
}

void Task_Yield() {
  // Pend PendSV interrupt
  SCB->ICSR |= SCB_ICSR_PENDSTSET_Msk;
  // Data syncronization barrier
  __DSB();
  // Instruction syncronization barrier
  __ISB();
}

extern void task2(void);
extern void _context_switch(uint32_t *task_stack);

void Scheduler_Init(void) {
  // No preemtion, only subpriority
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
  // Set the lowest priority for interrupts
  HAL_NVIC_SetPriority(PendSV_IRQn, 0b1111, 0);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0b1111, 0);
  HAL_NVIC_EnableIRQ(PendSV_IRQn);
  HAL_NVIC_EnableIRQ(SysTick_IRQn);

  Task_Create("task2", task2);
  _context_switch(tasks[0].sp);
}