#include "portable.h"
#include "spinlock.h"

static spinlock_t uart_spinlock;

void Print_Str(const char* s) {
  lock(&uart_spinlock);
  pUART_SendString(s);
  unlock(&uart_spinlock);
}