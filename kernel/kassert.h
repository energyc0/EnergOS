#ifndef KASSERT_H
#define KASSERT_H

#include "portable/irq.h"
#include "portable/uart.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define Panic(msg)                               \
  do {                                           \
    pDisable_IRQ();                              \
    pUART_SendString((const uint8_t*)"PANIC! "); \
    pUART_SendString((const uint8_t*)msg);       \
    pUART_SendString((const uint8_t*)"\n\r");    \
    while (1);                                   \
  } while (0)

#define kassert(expr)                                       \
  do {                                                      \
    if (!(expr))                                            \
      Panic("Assertion failed: " #expr " on file " __FILE__ \
            " and line " TOSTRING(__LINE__));               \
  } while (0)

#endif