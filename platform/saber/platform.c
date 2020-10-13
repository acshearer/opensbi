/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>

/*
 * Include these files as needed.
 * See config.mk PLATFORM_xxx configuration parameters.
 */
#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/serial/uart8250.h>
#include <sbi_utils/sys/clint.h>


#include "saber_tv.h"

#define SABER_PLIC_ADDR            0x20000000
#define SABER_TV_ADDR              0x10003000
#define SABER_CLINT_ADDR           0x10004000
#define SABER_UART_ADDR            0x10005000

#define SABER_PLIC_NUM_SOURCES     3
#define SABER_HART_COUNT           1

#define SABER_UART_INPUT_FREQ      10000000
#define SABER_UART_BAUDRATE        115200

static struct plic_data plic = {
  .addr = SABER_PLIC_ADDR,
  .num_src = SABER_PLIC_NUM_SOURCES,
};

static struct clint_data clint = {
  .addr = SABER_CLINT_ADDR,
  .first_hartid = 0,
  .hart_count = SABER_HART_COUNT,
  .has_64bit_mmio = FALSE,
};

struct saber_platdata {
  struct saber_tv_device* tv;
};


// Device data
struct saber_tv_device saber_tv;

// Platform data
static struct saber_platdata platdata = {
  .tv = &saber_tv
};



/*
 * Platform early initialization.
 */
static int saber_early_init(bool cold_boot)
{
  return 0;
}

/*
 * Platform final initialization.
 */
static int saber_final_init(bool cold_boot)
{
  return 0;
}

/*
 * Initialize the saber console.
 */
static int saber_console_init(void)
{
  platdata.tv->addr = SABER_TV_ADDR;
  platdata.tv->input_buffer = "help\n\r";
  platdata.tv->input_index = 0;

  return 0;

  /* Example if the generic UART8250 driver is used */
  // return uart8250_init(SABER_UART_ADDR, SABER_UART_INPUT_FREQ,
    //            SABER_UART_BAUDRATE, 0, 1);
}

/*
 * Write a character to the saber console output.
 */
static void saber_console_putc(char ch)
{
  _saber_tv_put_char(platdata.tv, ch);

  /* Example if the generic UART8250 driver is used */
  // uart8250_putc(ch);
}

/*
 * Read a character from the saber console input.
 */
static int saber_console_getc(void)
{
  // return uart8250_getc();
  return _saber_tv_get_char(platdata.tv);
}

/*
 * Initialize the saber interrupt controller for current HART.
 */
static int saber_irqchip_init(bool cold_boot)
{
  u32 hartid = current_hartid();
  int ret;

  /* Example if the generic PLIC driver is used */
  if (cold_boot) {
    ret = plic_cold_irqchip_init(&plic);
    if (ret)
      return ret;
  }

  return plic_warm_irqchip_init(&plic, 2 * hartid, 2 * hartid + 1);
}

/*
 * Initialize IPI for current HART.
 */
static int saber_ipi_init(bool cold_boot)
{
  int ret;

  /* Example if the generic CLINT driver is used */
  if (cold_boot) {
    ret = clint_cold_ipi_init(&clint);
    if (ret)
      return ret;
  }

  return clint_warm_ipi_init();
}

/*
 * Send IPI to a target HART
 */
static void saber_ipi_send(u32 target_hart)
{
  /* Example if the generic CLINT driver is used */
  clint_ipi_send(target_hart);
}

/*
 * Clear IPI for a target HART.
 */
static void saber_ipi_clear(u32 target_hart)
{
  /* Example if the generic CLINT driver is used */
  clint_ipi_clear(target_hart);
}

/*
 * Initialize saber timer for current HART.
 */
static int saber_timer_init(bool cold_boot)
{
  int ret;

  /* Example if the generic CLINT driver is used */
  if (cold_boot) {
    ret = clint_cold_timer_init(&clint, NULL);
    if (ret)
      return ret;
  }

  return clint_warm_timer_init();
}

/*
 * Get saber timer value.
 */
static u64 saber_timer_value(void)
{
  /* Example if the generic CLINT driver is used */
  return clint_timer_value();
}

/*
 * Start saber timer event for current HART.
 */
static void saber_timer_event_start(u64 next_event)
{
  /* Example if the generic CLINT driver is used */
  clint_timer_event_start(next_event);
}

/*
 * Stop saber timer event for current HART.
 */
static void saber_timer_event_stop(void)
{
  /* Example if the generic CLINT driver is used */
  clint_timer_event_stop();
}

/*
 * Reset the saber.
 */
static int saber_system_reset(u32 type)
{
  return 0;
}

/*
 * Platform descriptor.
 */
const struct sbi_platform_operations platform_ops = {
  .early_init            = saber_early_init,
  .final_init            = saber_final_init,
  .console_putc          = saber_console_putc,
  .console_getc          = saber_console_getc,
  .console_init          = saber_console_init,
  .irqchip_init          = saber_irqchip_init,
  .ipi_send              = saber_ipi_send,
  .ipi_clear             = saber_ipi_clear,
  .ipi_init              = saber_ipi_init,
  .timer_value           = saber_timer_value,
  .timer_event_stop      = saber_timer_event_stop,
  .timer_event_start     = saber_timer_event_start,
  .timer_init            = saber_timer_init,
  .system_reset          = saber_system_reset
};

const struct sbi_platform platform = {
  .opensbi_version       = OPENSBI_VERSION,
  .platform_version      = SBI_PLATFORM_VERSION(0x0, 0x00),
  .name                  = "saber",
  .features              = SBI_PLATFORM_DEFAULT_FEATURES,
  .hart_count            = 1,
  .hart_stack_size       = SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
  .platform_ops_addr     = (unsigned long)&platform_ops
};
