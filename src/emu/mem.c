/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
 * lc3tools - An implementation of the LC-3 ISA and assorted tools.           *
 * Copyright (C) 2018 Wes Hampson.                                            *
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
 *   Desc: Main memory for the LC-3b.
 *         Memory is word-addressable, but individual bytes can be written by
 *         manipulating the write mask.
 *============================================================================*/

#include <stdio.h>

#include <mem.h>
#include <keyboard.h>

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
            default:
                *data = m.d[addr >> 1];
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
                set_kbsr((get_kbsr() & ~wmask) | (data & wmask));
                break;
            case A_DDR:
            {
                /* Temp implementation... */
                putc(data & wmask, stdout);
                fflush(stdout);
                break;
            }
            default:
                m.d[addr >> 1] = (m.d[addr >> 1] & ~wmask) | (data & wmask);
                break;
        }
    }

    return !m.w_en;
}

void mem_read_nodelay(lc3word *data, lc3word addr)
{
    *data = m.d[addr >> 1];
}

void mem_write_nodelay(lc3word addr, lc3word data, lc3word wmask)
{
    m.d[addr >> 1] = (m.d[addr >> 1] & ~wmask) | (data & wmask);
}
