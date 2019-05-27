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
 *         loosely based on the Intel 8259 PIC.
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
    uint8_t irr;        /* interrupt request register */
    /* uint8_t isr; */       /* in-service register */
    uint8_t imr;        /* interrupt mask register */
};

/*
 * Reset PIC control signals.
 */
void pic_reset(void);

/*
 * Execute one clock cycle on the PIC.
 */
void pic_tick(void);

/*
 * Signal that a device requires service.
 *
 * @param num   the interrupt request number
 */
void raise_irq(int num);

/*
 * Mark that an interrupt has been serviced.
 *
 * @param num   the interrupt request number
 */
void finish_irq(int num);

/*
 * Disable interrupts on the specified IRQ line.
 *
 * @param num   the interrupt request number
 */
void mask_irq(int num);

/*
 * Enable interrupts on the specified IRQ line.
 *
 * @param num   the interrupt request number
 */
void unmask_irq(int num);

/*
 * Get the current value of the Interrupt Request Register.
 *
 * @return the current value in IRR
 */
uint8_t get_irr(void);

#endif /* __PIC_H */
