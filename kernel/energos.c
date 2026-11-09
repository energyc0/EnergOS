#include "task.h"
#include "user/printer.h"

void main() {
  Task_Create("p1", printer, "p1\n", T_KERNEL);
  Task_Create("p2", printer, "p2\n", T_KERNEL);
  // Task_Create("Shell", Shell_Start);
  Scheduler_Start();
}
