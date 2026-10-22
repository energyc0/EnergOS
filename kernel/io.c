#include <stdint.h>

#include "portable.h"
#include "uart_queue.h"

static const uint8_t k_hex[] = "0123456789ABCDEF";

static uint32_t Read_Char_Internal(char* c) {
  return pUART_ReceiveByte((uint8_t*)c, UINT32_MAX);
}

uint32_t Write_Str(const char* s) { return Uart_Queue_Write(s); }

uint32_t Write_Int(int32_t n) {
  uint8_t buf[12];
  uint8_t* p = buf + sizeof(buf) - 1;
  *p = '\0';

  uint32_t u;
  if (n < 0) {
    u = (uint32_t)(-(n + 1)) + 1u;
  } else {
    u = (uint32_t)n;
  }

  do {
    *--p = (uint8_t)('0' + (u % 10));
    u /= 10;
  } while (u != 0);

  if (n < 0) {
    *--p = '-';
  }

  return Uart_Queue_Write((const char*)p);
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
  uint8_t buf[11];
  buf[0] = '0';
  buf[1] = 'x';
  for (int i = 0; i < 8; i++) buf[2 + i] = k_hex[(v >> (28 - 4 * i)) & 0xFu];
  buf[10] = '\0';
  return Uart_Queue_Write((const char*)buf);
}
