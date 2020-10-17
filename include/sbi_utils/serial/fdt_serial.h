/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __FDT_SERIAL_H__
#define __FDT_SERIAL_H__

#include <sbi/sbi_types.h>

struct fdt_serial {
	const struct fdt_match *match_table;
	int (*init)(void *fdt, int nodeoff, const struct fdt_match *match);
	void (*putc)(char ch);
	int (*getc)(void);
};

void fdt_serial_putc(char ch);

int fdt_serial_getc(void);

int fdt_serial_init(void);

struct fdt_serial* get_current_ftd_serial_drvier();

struct fdt_serial* get_ftd_serial_driver(int i);

int get_ftd_serial_driver_count();

#endif
