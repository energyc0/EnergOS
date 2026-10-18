#ifndef PORTABLE_H
#define PORTABLE_H

#include <stdint.h>

/*Other*/
// Toggle the LED on the board for debugging
void pToggle_LED(void);

/* Scheduler */
uint32_t *pTask_Stack_Init(uint32_t *stack_top,
                           void (*task_entry)(int argc, char **argv));
void pScheduler_Init(void);
void pStart_First_Task(void);
void pJump_First_Task(void);
// High privilige task switch, used by the kernel to switch tasks
void pPend_Task_Switch(void);

/* Time */
void pDelay(uint32_t delay);

/* UART */
// Send a string to UART, return number of bytes sent,
//  return -1 on busy UART channel
uint32_t pUART_SendString(const uint8_t *str);
// Send a single byte to UART, return number of bytes sent,
//  return -1 on busy UART channel
uint32_t pUART_SendByte(uint8_t byte);
// Read a single byte from UART with blocking,
// returns 1 if a byte was read, 0 if timeout occurred,
// return -1 on busy UART channel
uint32_t pUART_ReceiveByte(uint8_t *byte, uint32_t timeout_ms);

uint32_t pSystem_Call(uint32_t sysnum);
uint32_t pSystem_Call_Arg1(uint32_t sysnum, uint32_t arg0);
uint32_t pSystem_Call_Arg2(uint32_t sysnum, uint32_t arg0, uint32_t arg1);
uint32_t pSystem_Call_Arg3(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                           uint32_t arg2);
uint32_t pSystem_Call_Arg4(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                           uint32_t arg2, uint32_t arg3);

/* Interrupts */
// Save the current IRQ state and disable interrupts
uint32_t pIRQ_Save(void);
// Restore the IRQ state to the given value
void pIRQ_Restore(uint32_t state);
void pDisable_IRQ(void);
void pEnable_IRQ(void);
// Return 1 on success configuring the system clock, 0 on failure
uint32_t pSetup_Timer_IRQ(void);

#endif