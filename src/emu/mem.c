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
 *   File: src/emu/mem.c
 * Author: Wes Hampson
 *   Desc: Main memory for the LC-3c.
 *         Memory is word-addressable, but individual bytes can be written by
 *         manipulating the write mask.
 *============================================================================*/

#include <stdio.h>

#include <mem.h>
#include <cpu.h>
#include <kbd.h>
#include <disp.h>
#include <pic.h>

/*
 * Overwrite the bits of a value based on a write mask.
 *
 * @param src   data with bits to overwrite
 * @param data  data with bits to be written
 * @param wmask write mask
 */
#define WRITE_BITS(src,data,wmask)  ((src & ~wmask) | (data & wmask))

static inline void do_read(lc3word *data, lc3word addr);
static inline void do_write(lc3word addr, lc3word data, lc3word wmask);

static struct lc3mem m;

void mem_reset(void)
{
    m.c = 0;
    m.r_en = 0;
    m.w_en = 0;
}

void mem_tick(void)
{
    if (m.c > 0) {
        m.c--;
    }
}

int mem_ready(void)
{
    return m.c == 0;
}

int mem_read(lc3word *data, lc3word addr)
{
    if (!m.r_en) {
        m.r_en = 1;
        m.c = MEM_DELAY;
    }
    else if (m.c == 0) {
        m.r_en = 0;
        switch (addr) {
            case A_KBSR:
                *data = get_kbsr();
                break;
            case A_KBDR:
                *data = get_kbdr();
                break;
            case A_DSR:
                *data = get_dsr();
                break;
            case A_ICDR:
                *data = get_icdr();
                break;
            case A_MCR:
                *data = get_mcr();
                break;
            default:
                do_read(data, addr);
                break;
        }
    }

    return !m.r_en;
}

int mem_write(lc3word addr, lc3word data, lc3word wmask)
{
    if (!m.w_en) {
        m.w_en = 1;
        m.c = MEM_DELAY;
    }
    else if (m.c == 0) {
        m.w_en = 0;
        switch (addr) {
            case A_KBSR:
                set_kbsr(WRITE_BITS(get_kbsr(), data, wmask));
                break;
            case A_DSR:
                set_dsr(WRITE_BITS(get_dsr(), data, wmask));
                break;
            case A_DDR:
                set_ddr(WRITE_BITS(get_ddr(), data, wmask));
                break;
            case A_ICCR:
                set_iccr(WRITE_BITS(get_iccr(), data, wmask));
                break;
            case A_ICDR:
                set_icdr(WRITE_BITS(get_icdr(), data, wmask));
                break;
            case A_MCR:
                set_mcr(WRITE_BITS(get_mcr(), data, wmask));
                break;
            default:
                do_write(addr, data, wmask);
                break;
        }
    }

    return !m.w_en;
}

void mem_read_nodelay(lc3word *data, lc3word addr)
{
    do_read(data, addr);
}

void mem_write_nodelay(lc3word addr, lc3word data, lc3word wmask)
{
    do_write(addr, data, wmask);
}

static inline void do_read(lc3word *data, lc3word addr)
{
    *data = m.d[addr >> 1];
}

static inline void do_write(lc3word addr, lc3word data, lc3word wmask)
{
    m.d[addr >> 1] = WRITE_BITS(m.d[addr >> 1], data, wmask);
}
