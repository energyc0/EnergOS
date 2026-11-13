#ifndef KERNEL_CONFIG_H
#define KERNEL_CONFIG_H

#define TASK_STACK_SIZE 512
#define NTASKS 16
#define F_CPU 16000000UL
// Timer tick in milliseconds
#define TIMER_TICK F_CPU / 1000 - 1

#define SHELL_MAX_LINE 128u
#define SHELL_MAX_ARGS 16u

#endif
