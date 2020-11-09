
#ifndef __SABER_PLATFORM_H__
#define __SABER_PLATFORM_H__

#include <sbi/sbi_types.h>
#include <sbi/sbi_trap.h>

void saber_notify_exception(const char* exception_name, u32 mcause, u32 mtval, struct sbi_trap_regs *regs);

#endif