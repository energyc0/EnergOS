#include "kassert.h"
#include "portable/stm32f411re/w25q64.h"
#include "portable/uart.h"
#include "stm32f411xe.h"
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
void main() {
  // Task_Create("p1", printer, (void *)&arg1, T_USER);
  // Task_Create("p2", printer, (void *)&arg2, T_USER);
  //  Task_Create("bad_task", Bad_Task, 0, T_USER);
  //  Task_Create("Shell", Shell_Start, 0, T_USER);
  W25Q64_Handle handler;
  SPI_HandleTypeDef hspi = {0};
  kassert(W25Q64_Init(&handler, &hspi, SPI1, GPIOA, 4) == 0);
  uint32_t jedec_id = W25Q64_ReadJEDEC_ID(&handler);
  pUart_Send_Hex(jedec_id);
  Scheduler_Start();
}
