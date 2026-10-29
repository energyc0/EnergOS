#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* All the syscalls return 0 on failure */

void Jump_First_Task(void);
// Return execution to another task.
void Yield(void);
// Terminate the current task.
void Exit(void);
// Create a new task with the given name and entry point.
// Returns the task ID of the new task.
uint32_t Create(const char *name, void (*entry)(int argc, char **argv));
// Pause exection of the current task for a specified number of ticks.
void Sleep(uint32_t ticks);
// Pause execution until scheduler won't wake the task up.
// void Pause();
// Try to wakeup the task from sleeping state.
uint32_t Wakeup(uint32_t tid);

#endif
