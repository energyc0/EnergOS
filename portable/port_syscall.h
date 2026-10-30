#ifndef PORT_SYSCALL_H
#define PORT_SYSCALL_H

#include <stdint.h>

uint32_t pSystem_Call(uint32_t sysnum);
uint32_t pSystem_Call_Arg1(uint32_t sysnum, uint32_t arg0);
uint32_t pSystem_Call_Arg2(uint32_t sysnum, uint32_t arg0, uint32_t arg1);
uint32_t pSystem_Call_Arg3(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                           uint32_t arg2);
uint32_t pSystem_Call_Arg4(uint32_t sysnum, uint32_t arg0, uint32_t arg1,
                           uint32_t arg2, uint32_t arg3);

#endif
