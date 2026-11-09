#include "printer.h"

#include "io.h"
#include "portable/time.h"
#include "sync.h"

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