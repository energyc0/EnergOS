#ifndef KASSERT_H
#define KASSERT_H

#include "portable.h"

#define panic(msg)              \
  do {                          \
    pDisable_IRQ();             \
    pUART_SendString("PANIC!"); \
    pUART_SendString(msg);      \
    pUART_SendString("\r\n");   \
    while (1);                  \
  } while (0)

#define kassert(expr)                           \
  do {                                          \
    if ((expr) == 0) panic("Assertion failed"); \
  } while (0)

#endif