
#ifndef __SABER_TV_H__
#define __SABER_TV_H__

#include <sbi/sbi_types.h>

void saber_tv_init(unsigned long addr_command, unsigned long abbr_data);

void saber_tv_putc(char ch);

int saber_tv_getc(void);

#endif