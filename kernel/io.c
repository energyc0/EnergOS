#include <stdint.h>

#include "portable.h"
#include "sync.h"

#define MAX_DELAY (0xFFFFFFFF)

static const char k_hex[] = "0123456789ABCDEF";

// static mutex_t uart_mutex;

int __Read_Char(char* c) { return pUART_ReceiveByte((uint8_t*)c, MAX_DELAY); }
void __Write_Char(char c) {
  if (c == '\n') {
    pUART_SendString("\r\n");
  } else if (c != '\r') {
    pUART_SendByte(c);
  }
}

void Write_Str(const char* s) {
  for (uint32_t i = 0; s[i] != '\0'; i++) {
    __Write_Char(s[i]);
  }
}

void Write_Int(int n) {
  char buf[12];
  char* p = buf + sizeof(buf) - 1;
  *p = '\0';

  unsigned int u;
  if (n < 0) {
    u = (unsigned int)(-(n + 1)) + 1u;
  } else {
    u = (unsigned int)n;
  }

  do {
    *--p = (char)('0' + (u % 10));
    u /= 10;
  } while (u != 0);

  if (n < 0) {
    *--p = '-';
  }

  pUART_SendString(p);
}

void Write_Char(char c) { __Write_Char(c); }

int Read_Char(char* c) {
  int ret = __Read_Char(c);
  return ret;
}

int Read_Str(char* buf, uint32_t size) {
  if (buf == 0 || size == 0) return -1;
  int ret = 0;
  uint32_t i = 0;
  for (; i < size - 1; i++) {
    ret = __Read_Char(&buf[i]);
    if (ret != 0) break;
    if (ret == 0 || buf[i] == '\r' || buf[i] == '\n') break;
  }
  buf[i] = '\0';
  return ret;
}

void Write_Hex(uint32_t v) {
  char buf[11];
  buf[0] = '0';
  buf[1] = 'x';
  for (int i = 0; i < 8; i++) buf[2 + i] = k_hex[(v >> (28 - 4 * i)) & 0xFu];
  buf[10] = '\0';
  pUART_SendString(buf);
}
