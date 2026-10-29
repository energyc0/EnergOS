
#include <stdint.h>

#include "cmsis_gcc.h"
#include "io.h"
#include "portable.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "syscall.h"

extern void Syscall_Handler(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                            uint32_t arg2, uint32_t arg3, uint32_t *ret);

typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t psr;
} HardFaultRegs_t;

void pPend_Task_Switch(void) {
  // Pend PendSV interrupt
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  // Data syncronization barrier
  __DSB();
  // Instruction syncronization barrier
  __ISB();
}

void SVC_Handler_C(uint32_t *stack_frame) {
  /*
      Exception stack frame has
      R0, R1, R2, R3, R12, LR, return address and RETPSR
      registers on the stack in order.
  */

  // r0 carries the syscall number; arguments occupy r1-r3 and r12.
  Syscall_Handler(stack_frame[0], stack_frame[1], stack_frame[2],
                  stack_frame[3], stack_frame[4], &stack_frame[0]);
}

void SysTick_Handler(void) {
  static uint32_t task_time_switch_ms = 0;
  HAL_IncTick();
  if (++task_time_switch_ms >= 50) {
    pPend_Task_Switch();
    task_time_switch_ms = 0;
  }
}

void HardFault_Handler(void) __attribute__((naked));
void HardFault_Handler(void) {
  __asm volatile(
      "tst lr, #4      \n"
      "ite eq          \n"
      "mrseq r0, msp   \n"
      "mrsne r0, psp   \n"
      "b HardFault_Handler_C \n");
}

void HardFault_Handler_C(HardFaultRegs_t *regs) {
  Write_Str("Hard Fault Detected!\n");
  Write_Str("PC (Faulting Instruction): ");
  Write_Hex(regs->pc);
  Write_Str("\nLR (Link Register): ");
  Write_Hex(regs->lr);
  Write_Str("\nCFSR: ");  // Configurable Fault Status Register
  Write_Hex(SCB->CFSR);
  Write_Str("\nHFSR: ");  // HardFault Status Register
  Write_Hex(SCB->HFSR);
  Write_Str("\nMMFAR: ");  // MemManage Fault Address Register
  Write_Hex(SCB->MMFAR);
  Write_Str("\nBFAR: ");
  Write_Hex(SCB->BFAR);  // BusFault Address Register
  Write_Char('\n');

  while (1);
}

uint32_t pIRQ_Save(void) {
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  return primask;
}
void pIRQ_Restore(uint32_t state) { __set_PRIMASK(state); }

void pDisable_IRQ(void) { __disable_irq(); }

void pEnable_IRQ(void) { __enable_irq(); }

uint32_t pSystem_Call(uint32_t sysnum) {
  register uint32_t r0 __asm__("r0") = sysnum;
  __asm volatile("svc 0" : "+r"(r0) : : "memory");
  return r0;
}

uint32_t pSystem_Call_Arg1(uint32_t sysnum, uint32_t arg0) {
  register uint32_t r0 __asm__("r0") = sysnum;
  register uint32_t r1 __asm__("r1") = arg0;
  __asm volatile("svc 0" : "+r"(r0) : "r"(r1) : "memory");
  return r0;
}

uint32_t pSystem_Call_Arg2(uint32_t sysnum, uint32_t arg0, uint32_t arg1) {
  register uint32_t r0 __asm__("r0") = sysnum;
  register uint32_t r1 __asm__("r1") = arg0;
  register uint32_t r2 __asm__("r2") = arg1;
  __asm volatile("svc 0" : "+r"(r0) : "r"(r1), "r"(r2) : "memory");
  return r0;
}

uint32_t pSystem_Call_Arg3(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                           uint32_t arg2) {
  register uint32_t r0 __asm__("r0") = sysnum;
  register uint32_t r1 __asm__("r1") = arg0;
  register uint32_t r2 __asm__("r2") = arg1;
  register uint32_t r3 __asm__("r3") = arg2;
  __asm volatile("svc 0" : "+r"(r0) : "r"(r1), "r"(r2), "r"(r3) : "memory");
  return r0;
}

uint32_t pSystem_Call_Arg4(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                           uint32_t arg2, uint32_t arg3) {
  register uint32_t r0 __asm__("r0") = sysnum;
  register uint32_t r1 __asm__("r1") = arg0;
  register uint32_t r2 __asm__("r2") = arg1;
  register uint32_t r3 __asm__("r3") = arg2;
  register uint32_t r12 __asm__("r12") = arg3;
  __asm volatile("svc 0"
                 : "+r"(r0)
                 : "r"(r1), "r"(r2), "r"(r3), "r"(r12)
                 : "memory");
  return r0;
}