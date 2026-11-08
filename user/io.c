#include <stdint.h>

#include "port_uart.h"
#include "uart_queue.h"
#include "util.h"

static uint32_t Read_Char_Internal(char* c) {
  return pUART_ReceiveByte((uint8_t*)c, UINT32_MAX);
}

uint32_t Write_Str(const char* s) { return Uart_Queue_Write(s); }

uint32_t Write_Int(int32_t n) {
  char buf[12];
  Util_Itoa(n, buf, sizeof(buf));
  return Uart_Queue_Write(buf);
}

uint32_t Write_Char(char c) { return Uart_Queue_Write_Char(c); }

uint32_t Read_Char(char* c) { return Read_Char_Internal(c); }

uint32_t Read_Str(char* buf, uint32_t size) {
  if (buf == 0 || size == 0) return -1;
  int ret = 0;
  uint32_t i = 0;
  for (; i < size - 1; i++) {
    ret = Read_Char_Internal(&buf[i]);
    if (ret != 0) break;
    if (ret == 0 || buf[i] == '\r' || buf[i] == '\n') break;
  }
  buf[i] = '\0';
  return ret;
}

uint32_t Write_Hex(uint32_t v) {
  char buf[11];
  Util_Itohex(v, buf, sizeof(buf));
  return Uart_Queue_Write(buf);
}
