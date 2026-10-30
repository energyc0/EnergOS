#ifndef PORT_TIME_H
#define PORT_TIME_H

#include <stdint.h>

uint32_t pGet_Tick(void);
void pDelay(uint32_t delay);
// Return 1 on success configuring the system clock, 0 on failure
uint32_t pSetup_Timer_IRQ(void);

#endif
