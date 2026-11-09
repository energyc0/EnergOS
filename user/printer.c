#include "printer.h"

#include "io.h"
#include "portable/time.h"
#include "sync.h"

mutex_t printer_mutex = MUTEX_INIT();

void printer(void* msg) {
  if (msg) {
    while (1) {
      mutex_lock(&printer_mutex);
      // uint32_t tick = pGet_Tick();
      Write_Str("Fuck niggers! ");
      Write_Str((const char*)msg);
      Write_Char('\n');
      mutex_unlock(&printer_mutex);
    }
  }
}