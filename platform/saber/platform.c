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

#include <saber/platform.h>
#include <saber/satori.h>

#define FDT_ADDRESS STRINGIFY(FW_FDT_LOCATION)

#define SABER_BANNER \
"  _____    __ ____  _____ ____\n"\
" / ____|  /  |  _ \\|  ___|  _ \\\n"\
"| (___   / . | |_) | |_  | |_) |\n"\
" \\___ \\ / /| |  _ <|  _| |    /\n"\
" ____) / __  | |_) | |___| |\\ \\\n"\
"|_____/_/  |_|____/|_____|_| \\_\\\n"


static int saber_early_init(bool cold_boot)
{
    return 0;
}

static int saber_final_init(bool cold_boot) {
    // multi_input_buffer_push_str(
    //     "ext4load mmc 0:1 0x82100000 /boot/saber.dtb\r\n"
    //     "ext4load mmc 0:1 0x81000000 /boot/uImage\r\n"
    //     // "md 0x81000000 4\r\n"
    //     "bootm 0x81000000 - 0x82100000\r\n"
    //     //"bootm 0x81000000\r\n"
    // );

    sbi_printf("\n"SABER_BANNER"\n");

    multi_serial_print_info();

    // saber_satori(0);

    return 0;
}

static int saber_system_reset(u32 type) {
	return 0;
}

void print_stack_trace(uint32_t stackPointer) {

    sbi_printf("Stack trace:\n");

    for(int i = 0; i < 256; i++) {
        uint32_t stackValue = *((uint32_t*)stackPointer);
        if(stackValue != 0) {
            sbi_printf(" @%8x:%8x ", stackPointer, stackValue);
        }
        stackPointer += 4;
    }

}

void saber_notify_illegal_instruction(u32 mcause, u32 mtval, u32 mepc, u32 sp) {
    sbi_printf("SBI trap: cause: %x, mtval: %x, mepc: %x\n", mcause, mtval, mepc);
    print_stack_trace(sp);
    sbi_printf("Press hardware continue to start memory inspector...");
    asm volatile("ebreak");
    saber_satori(mepc);
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
