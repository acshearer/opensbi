
#ifndef __MULTI_SERIAL_H__
#define __MULTI_SERIAL_H__

#include <sbi_utils/fdt/fdt_helper.h>

void multi_input_buffer_push_str(const char* str);

int multi_serial_init(void *fdt, int nodeoff, const struct fdt_match *match);

void multi_serial_putc(char ch);

int multi_serial_getc(void);

void multi_serial_print_info(void);

#endif