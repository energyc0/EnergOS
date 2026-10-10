#include <stdint.h>

#include "portable.h"
#include "spinlock.h"

#define MAX_DELAY (0xFFFFFFFF)

static spinlock_t uart_spinlock;

int __Read_Char(char* c) { return pUART_ReceiveByte((uint8_t*)c, MAX_DELAY); }
void __Write_Char(char c) {
  if (c == '\r') {
    pUART_SendString("\r\n");
  } else {
    pUART_SendByte(c);
  }
}

void Write_Str(const char* s) {
  lock(&uart_spinlock);
  pUART_SendString(s);
  unlock(&uart_spinlock);
}

void Write_Char(char c) {
  lock(&uart_spinlock);
  __Write_Char(c);
  unlock(&uart_spinlock);
}

int Read_Char(char* c) {
  lock(&uart_spinlock);
  int ret = __Read_Char(c);
  unlock(&uart_spinlock);
  return ret;
}

int Read_Str(char* buf, uint32_t size) {
  lock(&uart_spinlock);
  int ret = 0;
  uint32_t i = 0;
  for (; i < size - 1; i++) {
    ret = __Read_Char(&buf[i]);
    if (ret == 0 || buf[i] == '\r' || buf[i] == '\n') break;
  }
  buf[i] = '\0';
  unlock(&uart_spinlock);
  return ret;
}