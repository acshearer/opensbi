

#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/serial/saber-tv.h>
#include <sbi/sbi_string.h>

struct saber_tv_device {
	unsigned long addr_command;
	unsigned long addr_data;
};

static struct saber_tv_device device = {
    .addr_command = 0,
    .addr_data = 0
};

static void _saber_tv_set_data(struct saber_tv_device *device, u8 data) {
    *((u8*)(device->addr_data)) = data;
}

static u8 _saber_tv_get_data(struct saber_tv_device *device) {
    return *((u8*)(device->addr_data));
	return 0;
}

static void _saber_tv_run_command(struct saber_tv_device *device, u8 command) {
    *((u8*)(device->addr_command)) = command;
}

static void _saber_tv_new_line(struct saber_tv_device *device) {
	int x;
	int y;
	_saber_tv_run_command(device, TV_COMMAND_NEW_LINE);

	_saber_tv_run_command(device, TV_COMMAND_GET_Y);
	y = _saber_tv_get_data(device);

	_saber_tv_set_data(device, ' ');
	for(x = 0; x < TV_SCREEN_W; x++){
		_saber_tv_run_command(device, TV_COMMAND_PUT_CHAR);
	}
	
	_saber_tv_set_data(device, y);
	_saber_tv_run_command(device, TV_COMMAND_SET_Y);
}


static int _saber_tv_get_char(struct saber_tv_device *device) {
	return 0;
}


static void _saber_tv_put_char(struct saber_tv_device *device, char ch) {
	switch(ch){
		case '\n': 
			_saber_tv_new_line(device);
			break;
		
		case '\r':
			break;

		default:
			if(ch >= ' ' && ch <= '~'){
				_saber_tv_set_data(device, ch);
				_saber_tv_run_command(device, TV_COMMAND_PUT_CHAR);
			}
			break;
	}
}

int serial_saber_tv_init(void *fdt, int nodeoff, const struct fdt_match *match) {
	int rc;
	unsigned long addr;
    unsigned long size;

	rc = fdt_get_node_addr_size(fdt, nodeoff, &addr, &size);
	if (rc)
		return rc;

    device.addr_command = addr + TV_ADDR_VCR_OFFSET;
    device.addr_data = addr + TV_ADDR_VDR_OFFSET;

	return 0;
}


int serial_saber_tv_getc(void) {
    return _saber_tv_get_char(&device);
}


void serial_saber_tv_putc(char ch) {
    return _saber_tv_put_char(&device, ch);
}

static const struct fdt_match serial_saber_tv_match[] = {
	{ .compatible = "saber,saber-tv" },
	{ },
};

struct fdt_serial fdt_serial_saber_tv = {
	.match_table = serial_saber_tv_match,
	.init = serial_saber_tv_init,
	.getc = serial_saber_tv_getc,
	.putc = serial_saber_tv_putc
};
