#ifndef PRINTER_H
#define PRINTER_H

#include <stdint.h>
struct msg_and_time {
  const char* msg;
  uint32_t sleep_time;
};

void printer(void* msg_and_time);

#endif