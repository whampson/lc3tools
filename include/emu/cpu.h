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
 *   File: src/emu/cpu.h
 * Author: Wes Hampson
 *   Desc: LC-3b CPU logic.
 *============================================================================*/

#ifndef __CPU_H
#define __CPU_H

/*
 * Reset the CPU.
 *
 * On a reset, all signals are set to 0 except for the following:
 *   - State set to 18 (first stage of FETCH cycle)
 *   - R6 set to default supervisor stack pointer (0x3000)
 *   - Zero flag set to 1
 */
void cpu_reset(void);

/*
 * Execute one clock cycle.
 */
void cpu_tick(void);

#endif /* __CPU_H */
