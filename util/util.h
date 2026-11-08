#pragma once

#include <stddef.h>
#include <stdint.h>

size_t Util_Strlen(const char *str);
int Util_Strcmp(const char *left, const char *right);
int Util_Strncmp(const char *left, const char *right, size_t count);
size_t Util_Strlcpy(char *destination, const char *source,
                    size_t destination_size);

// Convert a signed decimal integer to a string.
// Returns the required length, excluding the null terminator.
// If the destination is too small, the result is truncated and null-terminated.
size_t Util_Itoa(int32_t value, char *destination, size_t destination_size);

// Convert an integer to "0x" followed by eight uppercase hexadecimal digits.
// Returns the required length, excluding the null terminator.
// If the destination is too small, the result is truncated and null-terminated.
size_t Util_Itohex(uint32_t value, char *destination, size_t destination_size);

void memcpy(void *dest, const void *src, size_t n);

void memset(void *dest, int value, size_t n);