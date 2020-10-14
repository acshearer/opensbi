/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>

#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/sys/clint.h>

#include <sbi_utils/serial/saber-tv.h>

#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/timer/fdt_timer.h>
#include <sbi_utils/ipi/fdt_ipi.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>

static int saber_early_init(bool cold_boot) {
  return 0;
}

static int saber_final_init(bool cold_boot) {
  saber_tv_set_input_buffer("help\n\r");
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
