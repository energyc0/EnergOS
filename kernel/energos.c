#include <stdint.h>

#include "io.h"
#include "portable.h"
#include "shell.h"
#include "sync.h"
#include "syscall.h"
#include "task.h"

mutex_t printer_mutex = MUTEX_INIT();

void printer(void* msg) {
  if (msg) {
    while (1) {
      mutex_lock(&printer_mutex);
      uint32_t tick = pGet_Tick();
      Write_Str("Ticks: ");
      Write_Int(tick);
      Write_Char('\n');
      Write_Str(msg);
      mutex_unlock(&printer_mutex);
    }
  }
}

void main() {
  Task_Create("p1", printer, "p1\n");
  Task_Create("p2", printer, "p2\n");
  // Task_Create("Shell", Shell_Start);
  Scheduler_Start();
}
