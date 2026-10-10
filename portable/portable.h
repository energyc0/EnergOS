#ifndef PORTABLE_H
#define PORTABLE_H

#include <stdint.h>

/* Scheduler */
uint32_t *pTask_Stack_Init(uint32_t *stack_top, void (*task_entry)(void));
void pScheduler_Init(void);
void _pScheduler_Start(uint32_t *stack_top, void (*task_entry)(void));
void pStart_First_Task(void);
void pJump_First_Task(void);
// Yield the current task and switch to the next one
void pTask_Yield(void);
// High privilige task switch, used by the kernel to switch tasks
void pPend_Task_Switch(void);

/* Time */
void pDelay(uint32_t delay);

/* UART */
void pUART_SendString(const char *str);
void pUART_SendByte(uint8_t byte);
// Read a single byte from UART with blocking,
// returns 1 if a byte was read, 0 if timeout occurred
int pUART_ReceiveByte(uint8_t *byte, uint32_t timeout_ms);

/* Interrupts */
void pDisable_IRQ(void);
void pEnable_IRQ(void);
int pSetup_Timer_IRQ(void);

#endif