/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
 * lc3tools - An implementation of the LC-3 ISA and assorted tools.           *
 * Copyright (C) 2018-2019 Wes Hampson.                                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details                                *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*==============================================================================
 *   File: srd/emu/disp.c
 * Author: Wes Hampson
 *   Desc: Display device driver.
 *============================================================================*/

#include <stdio.h>
#include <string.h>

#include <disp.h>
#include <pic.h>

#define RD()        (disp.dsr & DSR_RD)
#define SET_RD(x)   (disp.dsr = (x)?(disp.dsr|DSR_RD):(disp.dsr&~DSR_RD))

#define IE()        (disp.dsr & DSR_IE)
#define SET_IE(x)   (disp.dsr = (x)?(disp.dsr|DSR_IE):(disp.dsr&~DSR_IE))

static struct lc3disp disp;

void disp_reset(void)
{
    memset(&disp, 0, sizeof(struct lc3disp));

    SET_IE(0);
    SET_RD(1);
}

void disp_tick(void)
{
    if (disp.c > 0) {
        disp.c--;
    }

    if (!RD() && disp.c == 0) {
        putc(disp.ddr & 0xFF, stdout);
        fflush(stdout);
        SET_RD(1);
    }

    if (RD() && IE()) {
        raise_irq(DISP_IRQ);
    }
}

lc3word get_dsr(void)
{
    return disp.dsr;
}

void set_dsr(lc3word value)
{
    disp.dsr = value;
}

lc3word get_ddr(void)
{
    return disp.ddr;
}

void set_ddr(lc3word value)
{
    if (RD()) {
        disp.ddr = value;
        disp.c = DISP_DELAY;
        SET_RD(0);
    }
}
