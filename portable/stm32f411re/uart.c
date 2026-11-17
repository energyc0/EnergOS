#include "uart.h"

#include <stdint.h>

#include "stm32f4xx_hal.h"
#include "util/util.h"

#define INT_BUFFER_SIZE 12
#define HEX_BUFFER_SIZE 11

UART_HandleTypeDef huart2;

uint32_t pUart_Send(const uint8_t *data, uint32_t length) {
  if (length == 0) return 0;
  if (data == NULL) return 0;

  uint32_t sent = 0;
  while (sent < length) {
    uint32_t remaining = length - sent;
    uint16_t chunk_size =
        remaining > UINT16_MAX ? UINT16_MAX : (uint16_t)remaining;
    HAL_StatusTypeDef status = HAL_UART_Transmit(
        &huart2, (uint8_t *)&data[sent], chunk_size, HAL_MAX_DELAY);
    if (status == HAL_BUSY) {
      return sent == 0 ? UINT32_MAX : sent;
    }
    if (status != HAL_OK) {
      return sent;
    }
    sent += chunk_size;
  }
  return sent;
}

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

uint32_t pUART_SendByte(uint8_t byte) {
  HAL_StatusTypeDef status =
      HAL_UART_Transmit(&huart2, &byte, 1, HAL_MAX_DELAY);
  switch (status) {
    case HAL_OK:
      return 1;
    case HAL_BUSY:
      return -1;
    default:
      return 0;
  }
}

uint32_t pUART_SendString(const uint8_t *str) {
  if (str == NULL) return 0;
  uint16_t size = (uint16_t)Util_Strlen((const char *)str);
  HAL_StatusTypeDef status =
      HAL_UART_Transmit(&huart2, (uint8_t *)str, size, HAL_MAX_DELAY);
  switch (status) {
    case HAL_OK:
      return size;
    case HAL_BUSY:
      return -1;
    default:
      return 0;
  }
}

uint32_t pUART_Receive(uint8_t *buf, uint32_t size, uint32_t timeout_ms) {
  if (buf == NULL || size == 0) return 0;

  uint32_t bytes_read = 0;
  while (bytes_read < size) {
    uint32_t remaining = size - bytes_read;
    uint16_t chunk_size =
        remaining > UINT16_MAX ? UINT16_MAX : (uint16_t)remaining;
    HAL_StatusTypeDef status =
        HAL_UART_Receive(&huart2, &buf[bytes_read], chunk_size, timeout_ms);
    if (status == HAL_BUSY || status == HAL_ERROR) return bytes_read;

    bytes_read += chunk_size - huart2.RxXferCount;
    if (status != HAL_OK) return bytes_read;
  }
  return bytes_read;
}

uint32_t pUart_Send_Hex(uint32_t value) {
  char buffer[HEX_BUFFER_SIZE];
  Util_Itohex(value, buffer, sizeof(buffer));
  return pUART_SendString((const uint8_t *)buffer);
}

uint32_t pUart_Send_Int(int32_t value) {
  char buffer[INT_BUFFER_SIZE];
  Util_Itoa(value, buffer, sizeof(buffer));
  return pUART_SendString((const uint8_t *)buffer);
}