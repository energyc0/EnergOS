#ifndef PORT_UART_H
#define PORT_UART_H

#include <stdint.h>

// Send a string to UART, return number of bytes sent,
// return -1 on busy UART channel
uint32_t pUART_SendString(const uint8_t *str);
// Send a single byte, return number of bytes sent,
// return -1 on busy UART channel
uint32_t pUART_SendByte(uint8_t byte);
// Read a byte with blocking; returns 1 if read, 0 on timeout,
// and -1 if the UART channel is busy
uint32_t pUART_ReceiveByte(uint8_t *byte, uint32_t timeout_ms);

#endif
