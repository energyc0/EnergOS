#include "task.h"
#include "user/bad_task.h"
#include "user/printer.h"
#include "user/shell.h"

static const char task_message_1[] __attribute__((section(".user_rodata"))) =
    "p1\n";
static const char task_message_2[] __attribute__((section(".user_rodata"))) =
    "p2\n";

static const struct msg_and_time arg1
    __attribute__((section(".user_rodata"))) = {.msg = task_message_1,
                                                .sleep_time = 200};
static const struct msg_and_time arg2
    __attribute__((section(".user_rodata"))) = {.msg = task_message_2,
                                                .sleep_time = 400};

void main() {
  Task_Create("p1", printer, (void *)&arg1, T_USER);
  Task_Create("p2", printer, (void *)&arg2, T_USER);
  // Task_Create("bad_task", Bad_Task, 0, T_USER);
  // Task_Create("Shell", Shell_Start, 0, T_USER);
  Scheduler_Start();
}
