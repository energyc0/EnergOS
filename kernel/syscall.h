#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

void Jump_First_Task(void);
void Yield(void);
void Exit(void);
uint32_t Create(const char *name, void (*entry)(void));
void Syscall_Handler(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                     uint32_t arg2, uint32_t arg3, uint32_t *ret);

#endif
