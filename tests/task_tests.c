#include <stdint.h>

#include "kernel/sync.h"
#include "kernel/task.h"
#include "test_support.h"

extern uint32_t kernel_tasks_stack[NTASKS][TASK_STACK_SIZE];
extern uint32_t user_tasks_stack[NTASKS][TASK_STACK_SIZE];
extern task_t *current_task;

static void Entry(void *param) { (void)param; }

static uint32_t *last_stack;
static void (*last_entry)(void *param);
static void *last_param;
static task_t test_current_task;

uint32_t *pTask_Stack_Init(uint32_t *stack_top, void (*task_entry)(void *param),
                           void *param) {
  last_stack = stack_top;
  last_entry = task_entry;
  last_param = param;
  return stack_top;
}

int main(void) {
  int parameter = 42;
  test_current_task.state = T_RUNNING;
  current_task = &test_current_task;

  TEST_CHECK(Task_Create(0, Entry, &parameter, T_USER) == 0);
  TEST_CHECK(Task_Create("invalid", 0, &parameter, T_USER) == 0);
  TEST_CHECK(
      Task_Create("invalid", Entry, &parameter, (enum task_privilege)2) == 0);

  uint32_t first_tid = Task_Create("first", Entry, &parameter, T_USER);
  TEST_CHECK(last_stack == user_tasks_stack[0] + TASK_STACK_SIZE - 1);
  TEST_CHECK(last_entry == Entry && last_param == &parameter);
  uint32_t second_tid = Task_Create("second", Entry, 0, T_KERNEL);
  TEST_CHECK(last_stack == kernel_tasks_stack[1] + TASK_STACK_SIZE - 1);
  TEST_CHECK(last_entry == Entry && last_param == 0);
  uint32_t third_tid = Task_Create("third", Entry, 0, T_USER);
  TEST_CHECK(first_tid != 0 && second_tid != 0 && third_tid != 0);
  TEST_CHECK(first_tid != second_tid && second_tid != third_tid);

  task_t *first = Get_Task(first_tid);
  task_t *second = Get_Task(second_tid);
  task_t *third = Get_Task(third_tid);
  TEST_CHECK(first != 0 && second != 0 && third != 0);
  TEST_CHECK(first->state == T_READY && first->privilege == T_USER);
  TEST_CHECK(first->entry == Entry && first->name[0] == 'f');
  TEST_CHECK(second->sp == kernel_tasks_stack[1] + TASK_STACK_SIZE - 1);
  TEST_CHECK(second->privilege == T_KERNEL);
  TEST_CHECK(first->sp == user_tasks_stack[0] + TASK_STACK_SIZE - 1);

  for (uint32_t i = 3; i < NTASKS; i++) {
    TEST_CHECK(Task_Create("extra", Entry, 0, T_USER) != 0);
  }
  TEST_CHECK(Task_Create("full", Entry, 0, T_USER) == 0);

  Task_Delete(first);
  TEST_CHECK(first->state == T_UNUSED);
  uint32_t reused_tid = Task_Create("reused", Entry, 0, T_USER);
  TEST_CHECK(reused_tid != 0 && Get_Task(reused_tid) == first);

  TEST_PASS("task tests passed");
}
