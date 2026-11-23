#include <stdint.h>

#include "kernel/syscall_defs.h"
#include "portable/irq.h"
#include "portable/syscall.h"
#include "test_support.h"
#include "test_port.h"

static uint32_t captured_syscall;
static uint32_t captured_arg0;
static uint32_t captured_arg1;
static uint32_t captured_arg2;
static uint32_t captured_arg3;

static void Capture_SVC_Call(test_svc_frame_t *frame) {
  captured_syscall = frame->r0;
  captured_arg0 = frame->r1;
  captured_arg1 = frame->r2;
  captured_arg2 = frame->r3;
  captured_arg3 = frame->r12;

  frame->r0 = 0x12345678u ^ captured_syscall;
}

static void Test_No_Arguments(void) {
  uint32_t result = pSystem_Call(SYSCALL_GET_TICK);

  TEST_CHECK(captured_syscall == SYSCALL_GET_TICK);
  TEST_CHECK(result == (0x12345678u ^ SYSCALL_GET_TICK));
}

static void Test_Argument_Marshaling(void) {
  uint32_t arg0 = 0x11111111u;
  uint32_t arg1 = 0x22222222u;
  uint32_t arg2 = 0x33333333u;
  uint32_t arg3 = 0x44444444u;

  uint32_t result = pSystem_Call_Arg1(SYSCALL_SLEEP, arg0);
  TEST_CHECK(captured_syscall == SYSCALL_SLEEP);
  TEST_CHECK(captured_arg0 == arg0);
  TEST_CHECK(result == (0x12345678u ^ SYSCALL_SLEEP));

  result = pSystem_Call_Arg2(SYSCALL_CREATE, arg0, arg1);
  TEST_CHECK(captured_syscall == SYSCALL_CREATE);
  TEST_CHECK(captured_arg0 == arg0);
  TEST_CHECK(captured_arg1 == arg1);
  TEST_CHECK(result == (0x12345678u ^ SYSCALL_CREATE));

  result = pSystem_Call_Arg3(SYSCALL_CREATE, arg0, arg1, arg2);
  TEST_CHECK(captured_syscall == SYSCALL_CREATE);
  TEST_CHECK(captured_arg0 == arg0);
  TEST_CHECK(captured_arg1 == arg1);
  TEST_CHECK(captured_arg2 == arg2);
  TEST_CHECK(result == (0x12345678u ^ SYSCALL_CREATE));

  result = pSystem_Call_Arg4(SYSCALL_CREATE, arg0, arg1, arg2, arg3);
  TEST_CHECK(captured_syscall == SYSCALL_CREATE);
  TEST_CHECK(captured_arg0 == arg0);
  TEST_CHECK(captured_arg1 == arg1);
  TEST_CHECK(captured_arg2 == arg2);
  TEST_CHECK(captured_arg3 == arg3);
  TEST_CHECK(result == (0x12345678u ^ SYSCALL_CREATE));
}

int main(void) {
  Test_Port_Set_SVC_Handler(Capture_SVC_Call);
  Test_No_Arguments();
  Test_Argument_Marshaling();
  TEST_PASS("syscall tests passed");
}
