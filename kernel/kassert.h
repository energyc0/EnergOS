#pragma once

#include "portable.h"

#define panic()                     \
  do {                              \
    pDisable_IRQ();                 \
    pUART_SendString("PANIC!\r\n"); \
    while (1);                      \
  } while (0)

#define kassert(expr)         \
  do {                        \
    if ((expr) == 0) panic(); \
  } while (0)
