#ifndef PORTABLE_SYSCALL_H
#define PORTABLE_SYSCALL_H

#include <stdint.h>

__attribute__((section(".user_text"))) uint32_t pSystem_Call(uint32_t sysnum);
__attribute__((section(".user_text"))) uint32_t
pSystem_Call_Arg1(uint32_t sysnum, uint32_t arg0);
__attribute__((section(".user_text"))) uint32_t
pSystem_Call_Arg2(uint32_t sysnum, uint32_t arg0, uint32_t arg1);
__attribute__((section(".user_text"))) uint32_t
pSystem_Call_Arg3(uint32_t sysnum, uint32_t arg0, uint32_t arg1, uint32_t arg2);
__attribute__((section(".user_text"))) uint32_t
pSystem_Call_Arg4(uint32_t sysnum, uint32_t arg0, uint32_t arg1, uint32_t arg2,
                  uint32_t arg3);

#endif
