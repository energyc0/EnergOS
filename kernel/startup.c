#include <stdint.h>

#include "stm32f4xx_hal.h"

extern int main(void);
extern void PendSV_Handler(void);
extern void SVCall_Handler(void);
extern char _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void Reset_Handler() {
  uint32_t* source = &_sidata;
  uint32_t* destination;

  for (destination = &_sdata; destination < &_edata;) {
    *destination++ = *source++;
  }
  for (destination = &_sbss; destination < &_ebss;) {
    *destination++ = 0;
  }

  main();
  while (1);
}

void SysTick_Handler() { HAL_IncTick(); }

void HardFault_Handler() { while (1); }

__attribute__((section(".isr_vector"))) const uint32_t* isr_vector[] = {
    [0] = (uint32_t*)&_estack,          [1] = (uint32_t*)Reset_Handler,
    [3] = (uint32_t*)HardFault_Handler, [11] = (uint32_t*)SVCall_Handler,
    [14] = (uint32_t*)PendSV_Handler,   [15] = (uint32_t*)SysTick_Handler,
};
