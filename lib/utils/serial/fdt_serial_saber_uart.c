
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/saber-uart.h>
#include <saber/uart.h>

static int serial_saber_uart_init(void *fdt, int nodeoff, const struct fdt_match *match) {
	int rc;
	unsigned long addr;
    unsigned long size;

	rc = fdt_get_node_addr_size(fdt, nodeoff, &addr, &size);
	if (rc)
		return rc;

    unsigned long addr_tx_data    = addr + SABER_UART_TX_DATA_OFFSET;
    unsigned long addr_tx_control = addr + SABER_UART_TX_CONTROL_OFFSET;
    unsigned long addr_tx_status  = addr + SABER_UART_TX_STATUS_OFFSET;

	saber_uart_init(addr_tx_data, addr_tx_control, addr_tx_status);

	return 0;
}

static const struct fdt_match serial_saber_uart_match[] = {
	{ .compatible = "saber,saber-uart" },
	{ },
};

struct fdt_serial fdt_serial_saber_uart = {
	.match_table = serial_saber_uart_match,
	.init = serial_saber_uart_init,
	.getc = saber_uart_getc,
	.putc = saber_uart_putc
};
