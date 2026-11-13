#include "kernel/task.h"

#include <stdint.h>

#include "scheduler.h"
#include "stm32f4xx_hal.h"
#include "user/syscall.h"

extern void Zombie_Task(void);

uint32_t *pTask_Stack_Init(uint32_t *stack_top, void (*task_entry)(void *param),
                           void *param) {
  // Create exception frame
  stack_top -= 8;
  stack_top[7] = 0x1000000;             // xPSR, enable Thumb mode, 24 bit = 1
  stack_top[6] = (uint32_t)task_entry;  // PC
  stack_top[5] = (uint32_t)Exit;        // Return address of task
  stack_top[4] = 0;                     // R12
  stack_top[3] = 0;                     // R3
  stack_top[2] = 0;                     // R2
  stack_top[0] = (uint32_t)param;       // R0
  *(--stack_top) = 0xFFFFFFFD;  // Return to Thread mode using this task's PSP.
  stack_top -= 8;               // Reserve space for R4-R11

  return stack_top;
}

void pSet_Current_Task_Privilege(void) {
  task_t *task = Get_Current_Task();
  uint32_t control = 2u;  // Select PSP; privileged Thread mode is the default.

  if (task->privilege == T_USER) control |= 1u;

  __asm volatile("msr control, %0\n\tisb" : : "r"(control) : "memory");
}

void pScheduler_Init(void) {
  // No preemtion, only subpriority
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
  // Set the lowest priority for interrupts
  HAL_NVIC_SetPriority(PendSV_IRQn, 0b1111, 0);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0b1111, 0);
  HAL_NVIC_EnableIRQ(PendSV_IRQn);
  HAL_NVIC_EnableIRQ(SysTick_IRQn);
}
