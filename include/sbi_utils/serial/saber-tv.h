
#ifndef __SERIAL_SABER_TV_H__
#define __SERIAL_SABER_TV_H__

#include <sbi/sbi_types.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <saber/tv.h>

void saber_tv_set_input_buffer(const char* str);

int serial_saber_tv_init(void *fdt, int nodeoff, const struct fdt_match *match);
int serial_saber_tv_getc(void);
void serial_saber_tv_putc(char ch);


#endif