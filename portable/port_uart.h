#ifndef PORT_UART_H
#define PORT_UART_H

#include <stdint.h>

// Send a byte buffer. Returns the number of bytes sent; UINT32_MAX indicates
// the UART was busy before any bytes could be sent.
uint32_t pUart_Send(const uint8_t *data, uint32_t length);

// Send a string to UART, return number of bytes sent,
// return -1 on busy UART channel
uint32_t pUART_SendString(const uint8_t *str);

// Send a single byte, return number of bytes sent,
// return -1 on busy UART channel
uint32_t pUART_SendByte(uint8_t byte);

// Read a byte with blocking; returns 1 if read, 0 on timeout,
// and -1 if the UART channel is busy
uint32_t pUART_ReceiveByte(uint8_t *byte, uint32_t timeout_ms);

// Send the integer as "0x" followed by eight uppercase hexadecimal digits.
uint32_t pUart_Send_Hex(uint32_t value);

// Send the integer as a signed decimal string.
uint32_t pUart_Send_Int(int32_t value);

#endif
