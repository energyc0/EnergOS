#include <stdint.h>

#include "kernel/task.h"
#include "portable/irq.h"
#include "portable/scheduler.h"
#include "test_support.h"
#include "user/syscall.h"

static void Test_Entry(void *param) { (void)param; }

void Exit(void) {}

static void Test_Task_Stack_Frame(void) {
  uint32_t task_stack[32];
  uint32_t argument = 0x12345678;
  uint32_t *stack_top = &task_stack[31];
  uint32_t *saved_sp = pTask_Stack_Init(stack_top, Test_Entry, &argument);
  uint32_t *exception_frame = saved_sp + 9;

  TEST_CHECK(saved_sp == &task_stack[14]);
  TEST_CHECK(saved_sp[8] == 0xFFFFFFFD);
  TEST_CHECK(exception_frame[0] == (uint32_t)&argument);
  TEST_CHECK(exception_frame[2] == 0);
  TEST_CHECK(exception_frame[3] == 0);
  TEST_CHECK(exception_frame[4] == 0);
  TEST_CHECK(exception_frame[5] == (uint32_t)Exit);
  TEST_CHECK(exception_frame[6] == (uint32_t)Test_Entry);
  TEST_CHECK(exception_frame[7] == 0x01000000);
}

static uint32_t Read_Primask(void) {
  uint32_t value;
  __asm volatile("mrs %0, primask" : "=r"(value));
  return value;
}

static void Test_Irq_Save_Restore(void) {
  uint32_t previous = pIRQ_Save();
  TEST_CHECK((previous & 1u) == 0);
  TEST_CHECK((Read_Primask() & 1u) == 1);
  pIRQ_Restore(previous);
  TEST_CHECK((Read_Primask() & 1u) == 0);

  pDisable_IRQ();
  TEST_CHECK((Read_Primask() & 1u) == 1);
  pEnable_IRQ();
  TEST_CHECK((Read_Primask() & 1u) == 0);
}

int main(void) {
  Test_Task_Stack_Frame();
  Test_Irq_Save_Restore();
  TEST_PASS("Cortex-M4 port tests passed");
}
