#ifndef PORTABLE_IRQ_H
#define PORTABLE_IRQ_H

#include <stdint.h>

// Save the current IRQ state and disable interrupts
uint32_t pIRQ_Save(void);
// Restore the IRQ state to the given value
void pIRQ_Restore(uint32_t state);
void pDisable_IRQ(void);
void pEnable_IRQ(void);

#endif
