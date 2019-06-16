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
 *   File: include/emu/cpu.h
 * Author: Wes Hampson
 *   Desc: LC-3c CPU logic.
 *============================================================================*/

#ifndef __CPU_H
#define __CPU_H

#include <emu/lc3.h>

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

/*
 * Get the current value of INTF (boolean).
 *
 * @return the current value of INTF
 */
int cpu_intf(void);

/*
 * Get the current priority level.
 *
 * @return the current priority level
 */
int cpu_prio(void);

/*
 * Raise an interrupt on the CPU.
 *
 * @param vec   the interrupt vector
 * @param prio  the interrupt priority
 */
void cpu_interrupt(lc3byte vec, lc3byte prio);

/*
 * Dump the current register values to STDOUT.
 */
void cpu_dumpregs(void);

/*
 * Get the value of the Machine Control Register.
 *
 * @return      current value in MCR
 */
lc3word get_mcr(void);

/*
 * Set the value of the Machine Control Register.
 *
 * @param value the value to put in MCR
 */
void set_mcr(lc3word value);

#endif /* __CPU_H */
