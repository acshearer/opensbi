#include <sbi/sbi_string.h>
#include <sbi_utils/serial/saber-ps2kb.h>
#include <saber/ps2kb.h>

struct saber_tv_device {
	unsigned long addr_data;
};

static struct saber_tv_device device = {
    .addr_data = 0
};


static int _saber_ps2kb_get_char(struct saber_tv_device *device) {
    return *((u32*)(device->addr_data));
}

int saber_ps2kb_getc(void){
    return _saber_ps2kb_get_char(&device);
}

void saber_ps2kb_putc(char ch){
    // ignore
}

void saber_ps2kb_init(unsigned long addr_data){
    device.addr_data = addr_data;
}