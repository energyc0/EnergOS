#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdint.h>

#define TEST_WRITE0(text)                                                     \
  do {                                                                        \
    register uint32_t operation __asm("r0") = 0x04;                           \
    register const char *argument __asm("r1") = (text);                       \
    __asm volatile("bkpt 0xab" : "+r"(operation) : "r"(argument) : "memory"); \
  } while (0)

#define TEST_EXIT(status)                                                     \
  do {                                                                        \
    const uint32_t exit_block[2] = {0x20026, (status)};                       \
    register uint32_t operation __asm("r0") = 0x20;                           \
    register const uint32_t *argument __asm("r1") = exit_block;               \
    __asm volatile("bkpt 0xab" : "+r"(operation) : "r"(argument) : "memory"); \
    for (;;) {                                                                \
    }                                                                         \
  } while (0)

#define TEST_CHECK(expression)                \
  do {                                        \
    if (!(expression)) {                      \
      TEST_WRITE0("FAIL: " #expression "\n"); \
      TEST_EXIT(1);                           \
    }                                         \
  } while (0)

#define TEST_PASS(message)     \
  do {                         \
    TEST_WRITE0(message "\n"); \
    TEST_EXIT(0);              \
  } while (0)

#endif
