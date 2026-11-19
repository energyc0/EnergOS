#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* All the syscalls return 0 on failure */

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

// Pause execution of the current task for a specified number of ticks.
void Sleep(uint32_t ticks);

// Pause execution until scheduler won't wake the task up.
// void Pause();

// Try to wakeup the task from sleeping state.
uint32_t Wakeup(uint32_t tid);

// Wait for task to terminate and return.
// Returns the task ID of the terminated task, or 0 if the task ID is invalid.
uint32_t Wait(uint32_t tid);

// Read up to n bytes from UART into buf; returns the count read, possibly
// fewer than n if the receive times out.
uint32_t Read(void *buf, uint32_t n);

// Use system call to use HAL functions.
uint32_t Get_Tick();

// Use system call to access kernel task structure
uint32_t Get_Tid();

#endif
