#include "syscall.h"

#include <stdint.h>

#include "stm32f4xx_hal.h"
extern void Error_Handler(void);

void Pend_Task_Switch() {
  // Pend PendSV interrupt
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  // Data syncronization barrier
  __DSB();
  // Instruction syncronization barrier
  __ISB();
}

void SVCall_Handler_C(uint32_t* stack_frame) {
  /*
      Exception stack frame has
      R0, R2, R3, R12, LR, return address and RETPSR
      registers on the stack in order.
  */

  // Extract svc number from opcode (first 8 bit)
  uint32_t svc_id = (uint32_t)*((uint16_t*)stack_frame[6] - 1) & 0xFF;

  switch (svc_id) {
    case SYSCALL_TASK_SWITCH:
      Pend_Task_Switch();
      break;
    default:
      Error_Handler();
  }
}
