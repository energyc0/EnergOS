#include "task.h"

#include <stdint.h>

#include "kassert.h"
#include "portable.h"
#include "stm32f4xx_hal.h"
#include "syscall.h"

extern void Zombie_Task(void);

uint32_t *pTask_Stack_Init(uint32_t *stack_top, void (*task_entry)(void)) {
  // Create exception frame
  stack_top -= 8;
  stack_top[7] = 0x1000000;              // xPSR, enable Thumb mode, 24 bit = 1
  stack_top[6] = (uint32_t)task_entry;   // PC
  stack_top[5] = (uint32_t)Zombie_Task;  // Return address of task
  stack_top[4] = 0;                      // R12
  stack_top[3] = 0;                      // R3
  stack_top[2] = 0;                      // R2
  stack_top[1] = 0;                      // R1
  stack_top[0] = 0;                      // R0

  *(--stack_top) = 0xFFFFFFFD;  // EXC_RETURN
  stack_top -= 8;               // Reserve space for R4-R11

  return stack_top;
}

void _pScheduler_Start(uint32_t *stack_top, void (*task_entry)(void)) {
  (void)task_entry;
  __set_PSP((uint32_t)stack_top);

  pPend_Task_Switch();
  pEnable_IRQ();
  //   Unprivileged Handler Mode
  __set_CONTROL(0x3);
  while (1);
}

void pTask_Yield(void) {
  __asm volatile("SVC %0" : : "n"(SYSCALL_TASK_SWITCH));
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
