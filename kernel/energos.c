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
    Write_Str("a\n");
  }
}

void p2(int argc, char** argv) {
  (void)argc;
  (void)argv;
  while (1) {
    pToggle_LED();
    Write_Str(" b\n");
  }
}

void main() {
  Task_Create("p1", p1);
  Task_Create("p2", p2);
  // Task_Create("Shell", Shell_Start);
  Scheduler_Start();
}
