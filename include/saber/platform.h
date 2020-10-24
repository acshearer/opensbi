
#ifndef __SABER_PLATFORM_H__
#define __SABER_PLATFORM_H__

#include <sbi/sbi_types.h>

void saber_notify_illegal_instruction(u32 mcause, u32 mtval, u32 mepc, u32 sp);

#endif