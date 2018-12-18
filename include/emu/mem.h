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
 *   File: include/emu/mem.h
 * Author: Wes Hampson
 *   Desc: Main memory for the LC-3b.
 *         Memory is word-addressable, but individual bytes can be written by
 *         manipulating the write mask.
 *============================================================================*/

#ifndef __MEM_H
#define __MEM_H

#include <lc3.h>

/*
 * Memory size in bytes.
 */
#define MEM_SIZE        (1 << 16)

/*
 * Number of bits per cell (word size).
 */
#define MEM_WIDTH       16

/*
 * Number of cells (word count).
 */
#define MEM_DEPTH       ((MEM_SIZE) / (MEM_WIDTH / 8))

/*
 * Number of clock cycles to wait before reading or writing to memory
 * (to simulate slow hardware).
 */
#define MEM_DELAY       1

/*
 * Memory state.
 */
struct lc3mem {
    unsigned int c;             /* busy counter */
    int r_en;                   /* read enable flag */
    int w_en;                   /* write enable flag */
    lc3word d[MEM_DEPTH];       /* data (16-bit word addressable) */
};

/*
 * Reset control signals.
 */
void mem_reset(void);

/*
 * Execute one clock cycle on the memory unit.
 */
void mem_tick(void);

/*
 * Get a value indicating whether memory is idle.
 *
 * @return      if memory is idle
 *              0 if memory is busy performing a read or write
 */
int mem_ready(void);

/*
 * Read a word from memory.
 *
 * @param data  a pointer to store the value read
 * @param addr  the address to read from
 * @return      1 when reading is complete
 *              0 while data is being read
 */
int mem_read(lc3word *data, lc3word addr);

/* Write a word to memory.
 *
 * @param addr  the address to write to
 * @param data  the data to write
 * @param wmask a bitmask indicating which bits to overwrite
 * @return      1 when writing is complete
 *              0 while data is being written
 */
int mem_write(lc3word addr, lc3word data, lc3word wmask);

#endif /* __MEM_H */
