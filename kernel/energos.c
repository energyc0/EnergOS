#include "task.h"
#include "user/bad_task.h"
#include "user/printer.h"
#include "user/shell.h"
/*
static const char task_message_1[] __attribute__((section(".user_rodata"))) =
    "p1\n";
static const char task_message_2[] __attribute__((section(".user_rodata"))) =
    "p2\n";
*/
void main() {
  // Task_Create("p1", printer, (void *)task_message_1, T_USER);
  // Task_Create("p2", printer, (void *)task_message_2, T_USER);
  // Task_Create("bad_task", Bad_Task, 0, T_USER);
  Task_Create("Shell", Shell_Start, 0, T_USER);
  Scheduler_Start();
}
