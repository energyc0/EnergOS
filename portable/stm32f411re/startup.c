#include <stdint.h>

#include "kernel/kassert.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "util/util.h"

extern int main(void);
extern void PendSV_Handler(void);
extern void SVC_Handler(void);
extern int pSystemClock_Config(void);
extern int pUART_Init(void);
extern void SysTick_Handler(void);
extern void HardFault_Handler(void);
extern void MemFault_Handler(void);

extern char _estack;

extern uint32_t _sikerneldata;
extern uint32_t _siuserdata;

extern uint32_t _kernel_sdata;
extern uint32_t _kernel_edata;
extern uint32_t _user_sdata;
extern uint32_t _user_edata;

extern uint32_t __kernel_bss_start__;
extern uint32_t __kernel_bss_end__;
extern uint32_t __user_bss_start__;
extern uint32_t __user_bss_end__;

extern uint32_t __kernel_text_start;
extern uint32_t __kernel_text_end;
extern uint32_t __kernel_ram_start;
extern uint32_t __kernel_ram_end;

extern uint32_t __user_text_start;
extern uint32_t __user_text_end;
extern uint32_t __user_ram_start;
extern uint32_t __user_ram_end;

extern uint32_t __periph_start;
extern uint32_t __periph_end;
extern uint32_t __priv_periph_start;
extern uint32_t __priv_periph_end;

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

static void Configure_MPU_Region(uint8_t number, uint32_t base_address,
                                 uint8_t size, uint8_t access_permission,
                                 uint8_t disable_exec) {
  MPU_Region_InitTypeDef region = {0};

  region.Enable = MPU_REGION_ENABLE;
  region.Number = number;
  region.BaseAddress = base_address;
  region.Size = size;
  region.SubRegionDisable = 0;
  region.TypeExtField = MPU_TEX_LEVEL0;
  region.AccessPermission = access_permission;
  region.DisableExec = disable_exec;
  region.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  region.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  region.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&region);
}

void Init_MPU(void) {
  SCB->SHCSR |=
      (1 << SCB_SHCSR_BUSFAULTENA_Pos) | (1 << SCB_SHCSR_USGFAULTENA_Pos);

  HAL_MPU_Disable();

  Configure_MPU_Region(MPU_REGION_NUMBER0, (uint32_t)&__kernel_text_start,
                       MPU_REGION_SIZE_256KB, MPU_REGION_PRIV_RO,
                       MPU_INSTRUCTION_ACCESS_ENABLE);
  Configure_MPU_Region(MPU_REGION_NUMBER1, (uint32_t)&__user_text_start,
                       MPU_REGION_SIZE_256KB, MPU_REGION_PRIV_RO_URO,
                       MPU_INSTRUCTION_ACCESS_ENABLE);
  Configure_MPU_Region(MPU_REGION_NUMBER2, (uint32_t)&__kernel_ram_start,
                       MPU_REGION_SIZE_64KB, MPU_REGION_PRIV_RW,
                       MPU_INSTRUCTION_ACCESS_DISABLE);
  Configure_MPU_Region(MPU_REGION_NUMBER3, (uint32_t)&__user_ram_start,
                       MPU_REGION_SIZE_64KB, MPU_REGION_FULL_ACCESS,
                       MPU_INSTRUCTION_ACCESS_DISABLE);
  Configure_MPU_Region(MPU_REGION_NUMBER4, (uint32_t)&__periph_start,
                       MPU_REGION_SIZE_1MB, MPU_REGION_PRIV_RW,
                       MPU_INSTRUCTION_ACCESS_DISABLE);
  Configure_MPU_Region(MPU_REGION_NUMBER5, (uint32_t)&__priv_periph_start,
                       MPU_REGION_SIZE_1MB, MPU_REGION_PRIV_RW,
                       MPU_INSTRUCTION_ACCESS_DISABLE);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Reset_Handler() {
  memcpy(&_kernel_sdata, &_sikerneldata,
         (char*)&_kernel_edata - (char*)&_kernel_sdata);
  memcpy(&_user_sdata, &_siuserdata, (char*)&_user_edata - (char*)&_user_sdata);
  memset(&__kernel_bss_start__, 0,
         (char*)&__kernel_bss_end__ - (char*)&__kernel_bss_start__);
  memset(&__user_bss_start__, 0,
         (char*)&__user_bss_end__ - (char*)&__user_bss_start__);

  kassert(HAL_Init() == HAL_OK);
  GPIO_Init();
  pSystemClock_Config();
  pUART_Init();
  Init_MPU();

  main();
  while (1);
}

__attribute__((section(".isr_vector"))) const uint32_t* isr_vector[101] = {
    [0] = (uint32_t*)&_estack,         [1] = (uint32_t*)Reset_Handler,
    [4] = (uint32_t*)MemFault_Handler, [3] = (uint32_t*)HardFault_Handler,
    [11] = (uint32_t*)SVC_Handler,     [14] = (uint32_t*)PendSV_Handler,
    [15] = (uint32_t*)SysTick_Handler,
};
