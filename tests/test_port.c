#include "test_port.h"

static test_svc_handler_t svc_handler;

void Test_Port_Set_SVC_Handler(test_svc_handler_t handler) {
  svc_handler = handler;
}

void Test_SVC_Handler_C(test_svc_frame_t *frame) {
  if (svc_handler != 0) svc_handler(frame);
}

__attribute__((naked)) void Test_SVC_Handler(void) {
  __asm volatile(
      "tst lr, #4\n"
      "ite eq\n"
      "mrseq r0, msp\n"
      "mrsne r0, psp\n"
      "b Test_SVC_Handler_C");
}
