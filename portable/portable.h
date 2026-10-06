#pragma once

#include <stdint.h>

/* Scheduler */
uint32_t *pTask_Stack_Init(uint32_t *stack_top, void (*task_entry)(void));
void pScheduler_Init(void);
void pScheduler_Start(void);
void pTask_Yield(void);
void pPend_Task_Switch(void);

/* Time */
void pDelay(uint32_t delay);

/* UART */
void pUART_SendString(const char *str);
void pUART_SendByte(uint8_t byte);
int pUART_ReceiveByte(uint8_t *byte, uint32_t timeout_ms);

/* Interrupts */
void pDisable_IRQ(void);
int pSetup_Timer_IRQ(void);
