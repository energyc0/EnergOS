#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

void UART_Init(void);
void UART_SendString(const char *str);
void UART_SendByte(uint8_t byte);
bool UART_ReceiveByte(uint8_t *byte, uint32_t timeout_ms);