#ifndef TEST_PORT_H
#define TEST_PORT_H

#include <stdint.h>

typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t psr;
} test_svc_frame_t;

typedef void (*test_svc_handler_t)(test_svc_frame_t *frame);

void Test_Port_Set_SVC_Handler(test_svc_handler_t handler);

void Test_SVC_Handler(void);

#endif
