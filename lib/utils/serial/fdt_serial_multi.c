
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/multi-serial.h>

static int serial_multi_serial_init(void *fdt, int nodeoff, const struct fdt_match *match) {
	return multi_serial_init(fdt, nodeoff, match);
}


static const struct fdt_match multi_serial_match[] = {
	{ .compatible = "saber,saber-sbi-serial" },
	{ },
};

struct fdt_serial fdt_serial_multi = {
	.match_table = multi_serial_match,
	.init = serial_multi_serial_init,
	.getc = multi_serial_getc,
	.putc = multi_serial_putc
};