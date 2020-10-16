/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_string.h>

#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/sys/clint.h>

#include <sbi_utils/serial/saber-tv.h>

#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/timer/fdt_timer.h>
#include <sbi_utils/ipi/fdt_ipi.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>

#define SERIAL_DRIVER_COUNT 1

static char saber_input_buffer[256];
static int saber_input_buffer_index = -1;

struct serial_driver_info {
    const char* name;
    struct fdt_serial* driver;
    int status;
};

static struct serial_driver_info saber_serial_drivers[SERIAL_DRIVER_COUNT];
static const char* saber_serial_driver_names[SERIAL_DRIVER_COUNT] = {
    "tv0"
};

static int saber_early_init(bool cold_boot) {
    return 0;
}

static void saber_input_buffer_push(char c) {
    if(c > 0){
        saber_input_buffer_index++;
        saber_input_buffer[saber_input_buffer_index] = c;
    }
}

static void saber_input_buffer_push_str(const char* str) {
    int len = sbi_strlen(str);
    for(int i = len-1; i>=0; i--){
        saber_input_buffer_push(str[i]);
    }
}

static int saber_input_buffer_pop(void) {
    int c = 0;
    if(saber_input_buffer_index >= 0){
        c = saber_input_buffer[saber_input_buffer_index];
        saber_input_buffer_index--;
    }
    return c;
}

static void saber_serial_putc(char c) {
    for(int i = 0; i < SERIAL_DRIVER_COUNT; i++){
        if(!saber_serial_drivers[i].status){
            saber_serial_drivers[i].driver->putc(c);
        }
    }
}

static int saber_serial_getc(void) {
    for(int i = 0; i < SERIAL_DRIVER_COUNT; i++){
        if(!saber_serial_drivers[i].status){
            saber_input_buffer_push(saber_serial_drivers[i].driver->getc());
        }
    }
    return saber_input_buffer_pop();
}

static int saber_serial_init(void) {
    int rc;

    for(int i = 0; i < SERIAL_DRIVER_COUNT; i++){
        const char* name = saber_serial_driver_names[i];
        rc = fdt_serial_init_from_name(name);
        if (!rc) {
            saber_serial_drivers[i].driver = get_current_ftd_serial_drvier();
        }
        saber_serial_drivers[i].name = name;
        saber_serial_drivers[i].status = rc;
    }
    
    return 0;
}

static int saber_final_init(bool cold_boot) {
    saber_input_buffer_push_str("help\n\r");
    return 0;
}

static int saber_system_reset(u32 type) {
	return 0;
}

const struct sbi_platform_operations platform_ops = {
    .early_init            = saber_early_init,
    .final_init            = saber_final_init,

    .console_putc          = saber_serial_putc,
    .console_getc          = saber_serial_getc,
    .console_init          = saber_serial_init,

	.irqchip_init          = fdt_irqchip_init,

    .ipi_send              = fdt_ipi_send,
    .ipi_clear             = fdt_ipi_clear,
    .ipi_init              = fdt_ipi_init,
    
    .timer_value           = fdt_timer_value,
    .timer_event_start     = fdt_timer_event_start,
    .timer_event_stop      = fdt_timer_event_stop,
    .timer_init            = fdt_timer_init,

    .system_reset          = saber_system_reset
};

const struct sbi_platform platform = {
    .opensbi_version       = OPENSBI_VERSION,
    .platform_version      = SBI_PLATFORM_VERSION(1, 0),
    .name                  = "saber",
    .features              = SBI_PLATFORM_DEFAULT_FEATURES,
    .hart_count            = 1,
    .hart_stack_size       = SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
    .platform_ops_addr     = (unsigned long)&platform_ops
};
