#include <stdint.h>

#include "io.h"
#include "port_time.h"
#include "printer.h"
#include "shell.h"
#include "sync.h"
#include "syscall.h"
#include "task.h"

void main() {
  Task_Create("p1", printer, "p1\n", T_KERNEL);
  Task_Create("p2", printer, "p2\n", T_KERNEL);
  // Task_Create("Shell", Shell_Start);
  Scheduler_Start();
}
