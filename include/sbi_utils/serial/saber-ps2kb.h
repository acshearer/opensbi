
#ifndef __SABER_PS2KB_H__
#define __SABER_PS2KB_H__

#include <sbi/sbi_types.h>

void saber_ps2kb_init(unsigned long abbr_data);

void saber_ps2kb_putc(char ch);

int saber_ps2kb_getc(void);

#endif