#include <stdatomic.h>
#include <stdint.h>

#include "io.h"
#include "portable.h"
#include "shell.h"
#include "syscall.h"
#include "task.h"

void task1() {
  Exit();
  while (1) {
    pDelay(1);
  }
}
void task2() {
  while (1) {
    pDelay(1);
    Exit();
  }
}

void main() {
  Task_Create("task2", task2);
  Task_Create("task1", task1);
  Task_Create("Shell", Shell_Start);
  Scheduler_Start();
}
