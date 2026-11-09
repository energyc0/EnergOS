#ifndef PORTABLE_SCHEDULER_H
#define PORTABLE_SCHEDULER_H

#include <stdint.h>

uint32_t *pTask_Stack_Init(uint32_t *stack_top, void (*task_entry)(void *param),
                           void *param);
void pScheduler_Init(void);
void pStart_First_Task(void);
void pJump_First_Task(void);
void pSet_Current_Task_Privilege(void);

// High-priority task switch, used by the kernel to switch tasks
void pPend_Task_Switch(void);

#endif
