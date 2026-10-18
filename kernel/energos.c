#include <stdatomic.h>
#include <stdint.h>

#include "io.h"
#include "portable.h"
#include "shell.h"
#include "syscall.h"
#include "task.h"
void p1(int argc, char** argv) {
  (void)argc;
  (void)argv;
  while (1) {
    int status = Write_Str("a\n");
    if (status == -1) pDelay(5);
  }
}

void p2(int argc, char** argv) {
  (void)argc;
  (void)argv;
  while (1) {
    pToggle_LED();
    int status = Write_Str("b\n");
    if (status == -1) pDelay(5);
  }
}

void main() {
  Task_Create("p1", p1);
  Task_Create("p2", p2);
  // Task_Create("Shell", Shell_Start);
  Scheduler_Start();
}
