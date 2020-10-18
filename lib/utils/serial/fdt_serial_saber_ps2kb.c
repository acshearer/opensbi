

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/saber-ps2kb.h>
#include <saber/ps2kb.h>

static int serial_saber_ps2kb_init(void *fdt, int nodeoff, const struct fdt_match *match) {
	int rc;
	unsigned long addr;
    unsigned long size;

	rc = fdt_get_node_addr_size(fdt, nodeoff, &addr, &size);
	if (rc)
		return rc;

    unsigned long addr_data = addr + SABER_PS2KB_ADDR_DATA_OFFSET;

	saber_ps2kb_init(addr_data);

	return 0;
}

static const struct fdt_match serial_saber_ps2kb_match[] = {
	{ .compatible = "saber,saber-ps2kb" },
	{ },
};

struct fdt_serial fdt_serial_saber_ps2kb = {
	.match_table = serial_saber_ps2kb_match,
	.init = serial_saber_ps2kb_init,
	.getc = saber_ps2kb_getc,
	.putc = saber_ps2kb_putc
};
