
#ifndef __SABER_UART_H__
#define __SABER_UART_H__

#include <sbi/sbi_types.h>

void saber_uart_init(unsigned long addr_tx_data, unsigned long addr_tx_control, unsigned long addr_tx_status);

void saber_uart_putc(char ch);

int saber_uart_getc(void);

#endif