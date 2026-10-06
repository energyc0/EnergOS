#include <stdint.h>

#include "kassert.h"
#include "task.h"

/*
void GPIO_Init() {
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
*/

void task1() {
  while (1) {
    pDelay(500);
    pUART_SendString("1\r\n");
  }
}

void task2() {
  while (1) {
    pDelay(500);
    pUART_SendString("1\r\n");
  }
}

void main() {
  pUART_SendString("Hello!\r\n");

  pScheduler_Init();
  Task_Create("task1", task1);
  Task_Create("task2", task2);
  pScheduler_Start();
}
