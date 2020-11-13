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
#include <sbi/sbi_trap.h>

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


static int saber_early_init(bool cold_boot) {
    return 0;
}

void setBreakpoint(u32 index, u32 address) {
    *((u32*)(0x10002100)) |= 1 << index; // enable breakpoint
    *((u32*)(0x10002000 + (4*index))) = address; // set breakpint
}

static int saber_final_init(bool cold_boot) {
    multi_input_buffer_push_str(
        "setenv fdt_high 0xFFFFFFFF\n"
        "ext4load mmc 0:1 0x82100000 /boot/saber.dtb\n"
        "ext4load mmc 0:1 0x80400000 /boot/uImage\n"
        "bootm 0x80400000 - 0x82100000\n"
    );

    sbi_printf("\n"SABER_BANNER"\n");

    multi_serial_print_info();

    // setBreakpoint(0, 0x80400000);

    // saber_satori(0);

    return 0;
}

static int saber_system_reset(u32 type) {
	return 0;
}

void print_stack_trace(uint32_t stackPointer) {

    sbi_printf("Stack trace:\n");

    for(int i = 0; i < 64; i++) {
        uint32_t stackValue = *((uint32_t*)stackPointer);
        if(stackValue != 0) {
            sbi_printf(" @%8x:%8x ", stackPointer, stackValue);
        }
        stackPointer += 4;
    }

}

void saber_notify_exception(const char* exception_name, u32 mcause, u32 mtval, struct sbi_trap_regs *regs) {
    sbi_printf("SBI %s: mcause: %x, mtval: %x, mepc: %lx\n", exception_name, mcause, mtval, regs->mepc);
    sbi_printf("gp: %lx, sp: %lx, tp: %lx, ra: %lx\n", regs->gp, regs->sp, regs->tp, regs->ra);
    sbi_printf("a0 - a7: %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx\n", regs->a0, regs->a1, regs->a2, regs->a3, regs->a4, regs->a5, regs->a6, regs->a7);
    sbi_printf("s0 - s11: %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx\n", regs->s0, regs->s1, regs->s2, regs->s3, regs->s4, regs->s5, regs->s6, regs->s7, regs->s8, regs->s9, regs->s10, regs->s11);
    sbi_printf("t0 - t6: %lx, %lx, %lx, %lx, %lx, %lx, %lx\n", regs->t0, regs->t1, regs->t2, regs->t3, regs->t4, regs->t5, regs->t6);
    print_stack_trace(regs->sp);
    sbi_printf("\nPress escape to start satori memory inspector, or any other key to skip...\n");
    int c = 0;
    do{
        c = sbi_getc();
    }while(c <= 0);

    if(c == 27){
        saber_satori(regs->mepc);
    }
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
