
#include <sbi_utils/serial/saber-uart.h>
#include <saber/uart.h>


struct saber_uart_device {
	volatile uint8_t* TXDATA;
	volatile uint8_t* TXCTRL;
	volatile uint8_t* TXSTATUS;
};

static struct saber_uart_device device;

// static void _saber_uart_set_reg(unsigned long addr, u32 data) {
//     *((u32*)(addr)) = data;
// }

// static u32 _saber_uart_get_reg(unsigned long addr){
//     return *((u32*)(addr));
// }

static int _saber_uart_get_char(struct saber_uart_device *device){
    return 0;
}

static void _saber_uart_put_char(struct saber_uart_device *device, char ch){

    *device->TXDATA   = (uint8_t)ch;
    *device->TXCTRL   = (uint8_t)SABER_UART_TX_START;
    // Poll until poll clear state
    while (*device->TXSTATUS != (uint8_t)SABER_UART_TX_POLL_WAIT) {
    }
    *device->TXCTRL   = SABER_UART_TX_STOP;
    // Poll until Ready for next byte
    while (*device->TXSTATUS != (uint8_t)SABER_UART_TX_IDLE) {
    }
}


int saber_uart_getc(void){
    return _saber_uart_get_char(&device);
}

void saber_uart_putc(char ch){
    _saber_uart_put_char(&device, ch);
}

void saber_uart_init(unsigned long addr_tx_data, unsigned long addr_tx_control, unsigned long addr_tx_status){
    device.TXDATA = (uint8_t*)addr_tx_data;
    device.TXCTRL = (uint8_t*)addr_tx_control;
    device.TXSTATUS = (uint8_t*)addr_tx_status;
}