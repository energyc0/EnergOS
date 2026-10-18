#ifndef KASSERT_H
#define KASSERT_H

#include "portable.h"

#define panic(msg)                              \
  do {                                          \
    pDisable_IRQ();                             \
    pUART_SendString((const uint8_t*)"PANIC!"); \
    pUART_SendString((const uint8_t*)msg);      \
    pUART_SendString((const uint8_t*)"\n");     \
    while (1);                                  \
  } while (0)

#define kassert(expr)                           \
  do {                                          \
    if ((expr) == 0) panic("Assertion failed"); \
  } while (0)

#endif