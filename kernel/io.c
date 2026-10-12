#include <stdint.h>

#include "portable.h"
#include "sync.h"

#define MAX_DELAY (0xFFFFFFFF)

static mutex_t uart_mutex;

int __Read_Char(char* c) { return pUART_ReceiveByte((uint8_t*)c, MAX_DELAY); }
void __Write_Char(char c) {
  if (c == '\r' || c == '\n') {
    pUART_SendString("\r\n");
  } else {
    pUART_SendByte(c);
  }
}

void Write_Str(const char* s) {
  mutex_lock(&uart_mutex);
  pUART_SendString(s);
  mutex_unlock(&uart_mutex);
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

  mutex_lock(&uart_mutex);
  pUART_SendString(p);
  mutex_unlock(&uart_mutex);
}

void Write_Char(char c) {
  mutex_lock(&uart_mutex);
  __Write_Char(c);
  mutex_unlock(&uart_mutex);
}

int Read_Char(char* c) {
  mutex_lock(&uart_mutex);
  int ret = __Read_Char(c);
  mutex_unlock(&uart_mutex);
  return ret;
}

int Read_Str(char* buf, uint32_t size) {
  mutex_lock(&uart_mutex);
  int ret = 0;
  uint32_t i = 0;
  for (; i < size - 1; i++) {
    ret = __Read_Char(&buf[i]);
    if (ret == 0 || buf[i] == '\r' || buf[i] == '\n') break;
  }
  buf[i] = '\0';
  mutex_unlock(&uart_mutex);
  return ret;
}