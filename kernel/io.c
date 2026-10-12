#include <stdint.h>

#include "portable.h"
#include "sync.h"

#define MAX_DELAY (0xFFFFFFFF)

// static mutex_t uart_mutex;

int __Read_Char(char* c) { return pUART_ReceiveByte((uint8_t*)c, MAX_DELAY); }
void __Write_Char(char c) {
  if (c == '\r' || c == '\n') {
    pUART_SendString("\r\n");
  } else {
    pUART_SendByte(c);
  }
}

void Write_Str(const char* s) { pUART_SendString(s); }

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
  int ret = 0;
  uint32_t i = 0;
  for (; i < size - 1; i++) {
    ret = __Read_Char(&buf[i]);
    if (ret == 0 || buf[i] == '\r' || buf[i] == '\n') break;
  }
  buf[i] = '\0';
  return ret;
}