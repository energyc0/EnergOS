#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* All the syscalls return 0 on failure */

void Jump_First_Task(void);

// Return execution to another task.
void Yield(void);

// Terminate the current task.
void Exit(void);

// Create an unprivileged user task with the given name and entry point.
// Returns the task ID of the new task.
// param is passed unchanged; its storage must remain valid while the task uses
// it.
uint32_t Create(const char *name, void (*entry)(void *param), void *param);

void Print_Task_List(void);
// Pause exection of the current task for a specified number of ticks.
void Sleep(uint32_t ticks);

// Pause execution until scheduler won't wake the task up.
// void Pause();
// Try to wakeup the task from sleeping state.
uint32_t Wakeup(uint32_t tid);

// Wait for task to terminate and return.
// Returns the task ID of the terminated task, or 0 if the task ID is invalid.
uint32_t Wait(uint32_t tid);

#endif
