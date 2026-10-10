#include <stdatomic.h>
#include <stdint.h>

#include "io.h"
#include "portable.h"
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
  char buf[256];
  while (1) {
    // char c;
    // if (Read_Char(&c)) {
    //   Write_Char(c);
    // }

    Read_Str(buf, sizeof(buf));
    Write_Str(buf);
    pDelay(1);
  }
}
void task2() {
  while (1) {
    // Write_Str("task2\n\r");
    pDelay(1);
  }
}

void main() {
  Write_Str("Hello!\r\n");

  Task_Create("task2", task2);
  Task_Create("task1", task1);
  Scheduler_Start();
}
