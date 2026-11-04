#include <stdint.h>

#include "kassert.h"
#include "port_board.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

extern int main(void);
extern void PendSV_Handler(void);
extern void SVC_Handler(void);
extern int pSystemClock_Config(void);
extern int pUART_Init(void);
extern void SysTick_Handler(void);
extern void HardFault_Handler(void);
extern void MemFault_Handler(void);

extern char _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t __periph_start;
extern uint32_t __periph_end;

static void GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void pToggle_LED(void) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); }

// Must be a power of two!
static uint32_t Find_Bit(uint32_t value) {
  uint32_t power = 0;
  while (!(value & 1)) {
    value >>= 1;
    power++;
  }
  return power;
}

void Init_MPU(void) {
  // Enable MemFault, BusFault, UsageFault
  SCB->SHCSR |= (1 << SCB_SHCSR_MEMFAULTENA_Pos) |
                (1 << SCB_SHCSR_BUSFAULTENA_Pos) |
                (1 << SCB_SHCSR_USGFAULTENA_Pos);

  MPU->CTRL = 0;

  // PERIPHERY
  MPU->RNR = 0;
  MPU->RBAR = __periph_start;
  uint32_t periph_size =
      Find_Bit((uint8_t*)&__periph_end - (uint8_t*)&__periph_start);
  // ENABLE=1, SIZE=(periph_size<<1), AP=Priv RW(0x03<<24), XN=1
  MPU->RASR = (1 << 0) | (periph_size << 1) | (0x03 << 24) | (1 << 28);

  MPU->CTRL = (1 << 0) | (1 << 2);  // ENABLE=1, PRIVDEFENA=1

  __DSB();
  __ISB();
}

void Reset_Handler() {
  uint32_t* source = &_sidata;
  uint32_t* destination;

  for (destination = &_sdata; destination < &_edata;) {
    *destination++ = *source++;
  }
  for (destination = &_sbss; destination < &_ebss;) {
    *destination++ = 0;
  }

  kassert(HAL_Init() == HAL_OK);
  GPIO_Init();
  pSystemClock_Config();
  pUART_Init();
  Init_MPU();

  main();
  while (1);
}

__attribute__((section(".isr_vector"))) const uint32_t* isr_vector[] = {
    [0] = (uint32_t*)&_estack,         [1] = (uint32_t*)Reset_Handler,
    [4] = (uint32_t*)MemFault_Handler, [3] = (uint32_t*)HardFault_Handler,
    [11] = (uint32_t*)SVC_Handler,     [14] = (uint32_t*)PendSV_Handler,
    [15] = (uint32_t*)SysTick_Handler,
};
