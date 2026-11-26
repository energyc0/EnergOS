#include <stdint.h>

#include "kassert.h"
#include "portable/stm32f411re/w25q64.h"
#include "portable/uart.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "task.h"
#include "user/bad_task.h"
#include "user/printer.h"
#include "user/shell.h"
/*
static const char task_message_1[] __attribute__((section(".user_rodata"))) =
    "p1\n";
static const char task_message_2[] __attribute__((section(".user_rodata"))) =
    "p2\n";

static const struct msg_and_time arg1
    __attribute__((section(".user_rodata"))) = {.msg = task_message_1,
                                                .sleep_time = 200};
static const struct msg_and_time arg2
    __attribute__((section(".user_rodata"))) = {.msg = task_message_2,
                                                .sleep_time = 400};
*/

#define W25Q64_SPI SPI1
#define CS_PIN GPIO_PIN_4
#define CS_PORT GPIOA
#define CS_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define MY_VAR_VALUE (0xDEADBEEF)
#define MY_VAR_ADDR (0x00)

void main() {
  // Task_Create("p1", printer, (void *)&arg1, T_USER);
  // Task_Create("p2", printer, (void *)&arg2, T_USER);
  //  Task_Create("bad_task", Bad_Task, 0, T_USER);
  //  Task_Create("Shell", Shell_Start, 0, T_USER);
  W25Q64_Handle handler;
  SPI_HandleTypeDef hspi = {0};

  CS_GPIO_CLK_ENABLE();
  GPIO_InitTypeDef gpio_init = {0};
  gpio_init.Pin = CS_PIN;
  gpio_init.Pull = GPIO_PULLUP;
  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &gpio_init);

  kassert(W25Q64_Init(&handler, &hspi, W25Q64_SPI, CS_PORT, CS_PIN) == 0);
  uint32_t jedec_id = W25Q64_ReadJEDEC_ID(&handler);
  kassert(jedec_id == W25Q64_JEDEC_ID);

  uint32_t my_var = 0;
  pUART_SendString((uint8_t*)"First read:\n\r");
  W25Q64_Read(&handler, MY_VAR_ADDR, (uint8_t*)&my_var, sizeof(my_var));
  pUart_Send_Hex(my_var);
  pUART_SendString((uint8_t*)"\r\n");

  if (my_var != MY_VAR_VALUE) {
    W25Q64_EraseSector4KB(&handler, MY_VAR_ADDR);
    while (W25Q64_Is_Busy(&handler));
    my_var = MY_VAR_VALUE;
    W25Q64_Write(&handler, MY_VAR_ADDR, (uint8_t*)&my_var, sizeof(my_var));
    while (W25Q64_Is_Busy(&handler));
    my_var = 0;
    W25Q64_Read(&handler, MY_VAR_ADDR, (uint8_t*)&my_var, sizeof(my_var));
    pUART_SendString((uint8_t*)"Second read:\n\r");
    pUart_Send_Hex(my_var);
    pUART_SendString((uint8_t*)"\r\n");
  }
  Scheduler_Start();
}
