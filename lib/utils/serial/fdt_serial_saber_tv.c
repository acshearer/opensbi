

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/saber-tv.h>
#include <saber/tv.h>

static int serial_saber_tv_init(void *fdt, int nodeoff, const struct fdt_match *match) {
	int rc;
	unsigned long addr;
    unsigned long size;

	rc = fdt_get_node_addr_size(fdt, nodeoff, &addr, &size);
	if (rc)
		return rc;

    unsigned long addr_command = addr + TV_ADDR_VCR_OFFSET;
    unsigned long addr_data = addr + TV_ADDR_VDR_OFFSET;

	saber_tv_init(addr_command, addr_data);

	return 0;
}

static const struct fdt_match serial_saber_tv_match[] = {
	{ .compatible = "saber,saber-tv" },
	{ },
};

struct fdt_serial fdt_serial_saber_tv = {
	.match_table = serial_saber_tv_match,
	.init = serial_saber_tv_init,
	.getc = saber_tv_getc,
	.putc = saber_tv_putc
};
