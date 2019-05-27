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
 *   Desc: Programmable Interrupt Controller for the LC-3c.
 *         The PIC is not described in Patt & Patel's book. It is my own design
 *         loosely based on the Intel 8259 PIC.
 *============================================================================*/

#ifndef __PIC_H
#define __PIC_H

#include <stdint.h>

/*
 * Device interrupt service routine base vector.
 */
#define IRQ_BASE        0x80

#define PIC_CMD_IRR_R   0x01
#define PIC_CMD_ISR_R   0x02
#define PIC_CMD_IMR_R   0x03
#define PIC_CMD_IMR_W   0x04


/*
 * PIC state.
 */
struct lc3pic {
    uint8_t irr;        /* interrupt request register */
    uint8_t isr;        /* in-service register */
    uint8_t imr;        /* interrupt mask register */
    uint8_t iccr;       /* interrupt controller command register */
    uint8_t icdr;       /* interrupt controller data register */
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
 * Get the current value of the Interrupt Request Register.
 *
 * @return the current value in IRR
 */
uint8_t get_irr(void);

/*
 * Get the current value of the In-Service Register.
 *
 * @return the current value in ISR
 */
uint8_t get_isr(void);

/*
 * Get the current value of the Interrupt Mask Register.
 *
 * @return the current value in IMR
 */
uint8_t get_imr(void);

/*
 * Set the value of the Interrupt Mask Register.
 *
 * @param mask  the new IMR value
 */
void set_imr(uint8_t mask);

/*
 * Set the value of the Interrupt Controller Command Register.
 * This effectively issues a command to the interrupt controller.
 *
 * @param cmd   the new ICCR value
 */
void set_iccr(uint8_t cmd);

/*
 * Get the current value of the Interrupt Controller Data Register.
 *
 * @return the current value in ICDR
 */
uint8_t get_icdr(void);

/*
 * Set the value of the Interrupt Controller Data Register.
 *
 * @param data  the new ICDR value
 */
void set_icdr(uint8_t data);

#endif /* __PIC_H */
