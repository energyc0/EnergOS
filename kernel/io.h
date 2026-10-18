#ifndef IO_H
#define IO_H

/*
    High level I/O functions for UART communication.
    For example, translate '\n' to '\r\n' when writing strings, etc.
 */
#include <stdint.h>

// Write a string to UART, returns the number of bytes written
uint32_t Write_Str(const char* s);
// Write a single character to UART, returns the number of bytes written
uint32_t Write_Char(char c);
// Write an integer to UART, returns the number of bytes written
uint32_t Write_Int(int32_t n);
// Write a hexadecimal representation of an integer to UART, returns the number
// of bytes written
uint32_t Write_Hex(uint32_t v);

// Read a single byte from UART with blocking,
// returns 1 if a byte was read, 0 if timeout occurred
uint32_t Read_Char(char* c);

// Read a string from UART with blocking,
// returns the number of bytes read, or 0 if timeout occurred.
// The string will be null-terminated if at least one byte was read.
uint32_t Read_Str(char* buf, uint32_t size);

#endif