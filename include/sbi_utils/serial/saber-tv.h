
#ifndef __SERIAL_SABER_TV_H__
#define __SERIAL_SABER_TV_H__

#define TV_ADDR_VCR_OFFSET 0x0
#define TV_ADDR_VDR_OFFSET 0x4

#define TV_SCREEN_W 80
#define TV_SCREEN_H 60

#define TV_COMMAND_SET_CHAR 0b00000001
#define TV_COMMAND_ADVANCE  0b00000010
#define TV_COMMAND_PUT_CHAR 0b00000011
#define TV_COMMAND_SET_X    0b00000100
#define TV_COMMAND_SET_Y    0b00000101
#define TV_COMMAND_NEW_LINE 0b00001000

#define TV_COMMAND_GET_X    0b00010100
#define TV_COMMAND_GET_Y    0b00010101

#include <sbi/sbi_types.h>
#include <sbi_utils/fdt/fdt_helper.h>

void saber_tv_set_input_buffer(const char* str);

int serial_saber_tv_init(void *fdt, int nodeoff, const struct fdt_match *match);
int serial_saber_tv_getc(void);
void serial_saber_tv_putc(char ch);


#endif