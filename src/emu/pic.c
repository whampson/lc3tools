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
 *   File: include/emu/pic.c
 * Author: Wes Hampson
 *   Desc: Programmable Interrupt Controller for the LC-3b.
 *         The PIC is not described in Patt & Patel's book. It is my own design
 *         loosely based on the Intel-8259 PIC.
 *============================================================================*/

#include <cpu.h>
#include <pic.h>

static struct lc3pic pic;

void pic_reset(void)
{
    pic.irr = 0;
    /* pic.isr = 0; */
    pic.imr = 0;
}

void pic_tick(void)
{
    int curr_prio;

    /* Check for pending interrupts.
       If a pending interrupt is detected, INTP is set to the interrupt's
       priority level, INTV is set to the interrupt's vector number, and INTF is
       set to 1. Only interrupts with a higher priority than the current-running
       process's priority will be acknowledged. The interrupt priority is
       encoded in the IRQ bitmask:
           IR7..IR0 <=> PL7..PL0
       A higher PL number indicates higher priority. */

    curr_prio = 7;
    while (!cpu_intf() && curr_prio >= 0) {
        if ((pic.irr & (1 << curr_prio)) && curr_prio > cpu_prio()) {
            cpu_interrupt(IRQ_BASE | curr_prio, curr_prio);
        }
        curr_prio--;
    }
}

void raise_irq(int num)
{
    pic.irr |= (1 << (num & 7)) & ~pic.imr;
}

void finish_irq(int num)
{
    pic.irr &= ~(1 << (num & 7));
}

void mask_irq(int num)
{
    pic.imr |= 1 << (num & 7);
}

void unmask_irq(int num)
{
    pic.imr &= ~(1 << (num & 7));
}

uint8_t get_irr(void)
{
    return pic.irr;
}
