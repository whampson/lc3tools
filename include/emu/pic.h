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
 *   File: include/emu/pic.h
 * Author: Wes Hampson
 *   Desc: Programmable Interrupt Controller for the LC-3b.
 *         The PIC is not described in Patt & Patel's book. It is my own design
 *         loosely based on the Intel-8259 PIC.
 *============================================================================*/

#ifndef __PIC_H
#define __PIC_H

#include <stdint.h>

/*
 * Device interrupt service routine base vector.
 */
#define IRQ_BASE    0x80

/*
 * PIC state.
 */
struct lc3pic {
    uint8_t irq_mask;   /* bitmask of interrupts pending service */
};

/*
 * Reset PIC control signals.
 */
void pic_reset(void);

/*
 * Signal that a device requires service.
 *
 * @param num   the interrupt request number
 */
void raise_irq(int num);

/*
 * Mark that a device has finished being serviced.
 *
 * @param num   the interrupt request number
 */
void clear_irq(int num);

/*
 * Get a copy of the current IRQ mask.
 * Each high bit in the mask corresponds to a device that requires service.
 *
 * @return      the interrupt request mask
 */
uint8_t get_irq_mask(void);

#endif /* __PIC_H */
