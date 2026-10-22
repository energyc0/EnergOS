#ifndef UART_TX_H
#define UART_TX_H

#include <stdint.h>

/*
    Put characters into UART queue,
    then separate task transmits them
*/

// Init UART transmitter task
void Uart_Queue_Init(void);
uint32_t Uart_Queue_Write(const char *str);
uint32_t Uart_Queue_Write_Char(char c);

#endif
