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
#include <sbi/sbi_console.h>

#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/sys/clint.h>

#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/timer/fdt_timer.h>
#include <sbi_utils/ipi/fdt_ipi.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>

#include <sbi_utils/serial/multi-serial.h>

#define SABER_BANNER \
"  _____    __ ____  _____ ____\n"\
" / ____|  /  |  _ \\|  ___|  _ \\\n"\
"| (___   / . | |_) | |_  | |_) )\n"\
" \\___ \\ / /| |  _ <|  _| |    /\n"\
" ____) / __  | |_) | |___| |\\ \\\n"\
"|_____/_/  |_|____/|_____|_| \\_\\\n"


static int saber_early_init(bool cold_boot)
{
    return 0;
}

static int saber_final_init(bool cold_boot) {
    multi_input_buffer_push_str("help\n\r");

    sbi_printf("\n"SABER_BANNER"\n");

    multi_serial_print_info();

    // const char* str = "\033[31m";

    // for(int i = 0; str[i]; i++){
    //     fdt_serial_putc(str[i]);
    // }

    return 0;
}

static int saber_system_reset(u32 type) {
	return 0;
}

const struct sbi_platform_operations platform_ops = {
    .early_init            = saber_early_init,
    .final_init            = saber_final_init,

    .console_putc          = fdt_serial_putc,
    .console_getc          = fdt_serial_getc,
    .console_init          = fdt_serial_init,

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
