#ifndef IO_H
#define IO_H

/*
    High level I/O functions for UART communication.
    For example, translate '\n' to '\r\n' when writing strings, etc.
 */
#include <stdint.h>

void Write_Str(const char* s);
void Write_Char(char c);

// Read a single byte from UART with blocking,
// returns 1 if a byte was read, 0 if timeout occurred
int Read_Char(char* c);

// Read a string from UART with blocking,
// returns the number of bytes read, or 0 if timeout occurred.
// The string will be null-terminated if at least one byte was read.
int Read_Str(char* buf, uint32_t size);

#endif