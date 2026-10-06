#pragma once

#include <stdint.h>

typedef struct task task_t;

// Creates task and returns its id
// Valid tid is a value more than 0
// Return 0 on error
uint32_t Task_Create(const char *name, void (*entry)(void));

void Task_Yield(void);

void Scheduler_Start(void);