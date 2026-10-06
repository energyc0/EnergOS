
#include <stdint.h>

#include "cmsis_gcc.h"
#include "portable.h"
#include "stm32f411xe.h"

extern void Syscall_Handler(uint32_t);

void pPend_Task_Switch(void) {
  // Pend PendSV interrupt
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  // Data syncronization barrier
  __DSB();
  // Instruction syncronization barrier
  __ISB();
}

void SVC_Handler_C(uint32_t* stack_frame) {
  /*
      Exception stack frame has
      R0, R2, R3, R12, LR, return address and RETPSR
      registers on the stack in order.
  */

  // Extract svc number from opcode (first 8 bit)
  uint32_t svc_id = (uint32_t)*((uint16_t*)stack_frame[6] - 1) & 0xFF;
  // Enter kernel
  Syscall_Handler(svc_id);
}

void pDisable_IRQ() { __disable_irq(); }