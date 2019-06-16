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
 *   File: include/emu/kbd.h
 * Author: Wes Hampson
 *   Desc: Keyboard input device driver.
 *============================================================================*/

#ifndef __KBD_H
#define __KBD_H

#include <emu/lc3.h>

/*
 * Keyboard IRQ line (interrupt priority).
 */
#define KBD_IRQ     4

/*
 * Keyboard state.
 */
struct lc3kbd {
    lc3word kbsr;   /* status register */
    lc3word kbdr;   /* data register */
};

/*
 * Reset the keyboard state.
 */
void kbd_reset(void);

/*
 * Execute one clock cycle on the keyboard.
 */
void kbd_tick(void);

/*
 * Get the value of the Keyboard Status Register.
 *
 * @return      current value in KBSR
 */
lc3word get_kbsr(void);

/*
 * Set the value of the Keyboard Status Register.
 *
 * @param value the value to put in KBSR
 */
void set_kbsr(lc3word value);

/*
 * Get the value of the Keyboard Data Register.
 *
 * @return      current value in KBDR
 */
lc3word get_kbdr(void);

/*
 * Set the value of the Keyboard Data Register.
 *
 * @param value the value to put in KBDR
 */
void set_kbdr(lc3word value);

#endif /* __KBD_H */
