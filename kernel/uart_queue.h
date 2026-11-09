#ifndef UART_QUEUE_H
#define UART_QUEUE_H

#include <stdint.h>

/*
    Put characters into UART queue,
    then separate task transmits them
*/

// Init UART transmitter task
void Uart_Queue_Init(void);
__attribute__((section(".user_text"))) uint32_t
Uart_Queue_Write(const char *str);
__attribute__((section(".user_text"))) uint32_t Uart_Queue_Write_Char(char c);

#endif
