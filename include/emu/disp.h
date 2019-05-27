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
 *   File: include/emu/disp.h
 * Author: Wes Hampson
 *   Desc: Display device driver.
 *============================================================================*/

#ifndef __DISP_H
#define __DISP_H

#include <lc3.h>

/*
 * Display IRQ line (interrupt priority).
 */
#define DISP_IRQ    3

/*
 * Display state.
 */
struct lc3disp {
    lc3word dsr;    /* status register */
    lc3word ddr;    /* data register */
};

/*
 * Reset the display state.
 */
void disp_reset(void);

/*
 * Execute one clock cycle on the display device.
 */
void disp_tick(void);

/*
 * Get the value of the Display Status Register.
 *
 * @return      current value in DSR
 */
lc3word get_dsr(void);

/*
 * Set the value of the Display Status Register.
 *
 * @param value the value to put in DSR
 */
void set_dsr(lc3word value);

/*
 * Get the value of the Display Data Register.
 *
 * @return      current value in DDR
 */
lc3word get_ddr(void);

/*
 * Set the value of the Display Data Register.
 *
 * @param value the value to put in DDR
 */
void set_ddr(lc3word value);

#endif /* __DISP_H */
