#ifndef PORTABLE_TIME_H
#define PORTABLE_TIME_H

#include <stdint.h>

// High privilege function to get the current tick count from the system timer.
uint32_t pGet_Tick(void);

// Return 1 on success configuring the system clock, 0 on failure
uint32_t pSetup_Timer_IRQ(void);

#endif
