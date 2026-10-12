#include <stdatomic.h>
#include <stdint.h>

#include "io.h"
#include "portable.h"
#include "shell.h"
#include "task.h"

extern void Syscall_Exit(void);

void task1() {
  Syscall_Exit();
  while (1) {
    pDelay(1);
  }
}
void task2() {
  while (1) {
    pDelay(1);
    Syscall_Exit();
  }
}

void main() {
  Task_Create("task2", task2);
  Task_Create("task1", task1);
  Task_Create("Shell", Shell_Start);
  Scheduler_Start();
}
