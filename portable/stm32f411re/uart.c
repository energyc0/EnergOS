#include <stdint.h>
#include <string.h>

#include "portable.h"
#include "stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

int pUART_Init(void) {
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_USART2_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart2.Instance = USART2;
  huart2.Init.BaudRate = BAUDRATE;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  return HAL_UART_Init(&huart2) == HAL_OK;
}

void pUART_SendByte(uint8_t byte) {
  HAL_UART_Transmit(&huart2, &byte, 1, HAL_MAX_DELAY);
}

void pUART_SendString(const char *str) {
  if (str == NULL) return;
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

int pUART_ReceiveByte(uint8_t *byte, uint32_t timeout_ms) {
  if (byte == NULL) return 0;
  return (HAL_UART_Receive(&huart2, byte, 1, timeout_ms) == HAL_OK);
}