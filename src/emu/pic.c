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
 *   File: include/emu/pic.c
 * Author: Wes Hampson
 *   Desc: Programmable Interrupt Controller for the LC-3c.
 *         The PIC is not described in Patt & Patel's book. It is my own design
 *         loosely based on the Intel-8259 PIC.
 *============================================================================*/

#include <string.h>

#include <pic.h>
#include <cpu.h>

#define SET_BIT(val,pos)    (val|=(1 <<(pos)))
#define CLEAR_BIT(val,pos)  (val&=~(1 <<(pos)))
#define IS_BIT_SET(val,pos) ((val&(1<<(pos)))!=0)

static struct lc3pic pic;

void pic_reset(void)
{
    memset(&pic, 0, sizeof(struct lc3pic));
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
        if (curr_prio > cpu_prio() && IS_BIT_SET(pic.irr, curr_prio)) {
            CLEAR_BIT(pic.irr, curr_prio);
            SET_BIT(pic.isr, curr_prio);
            cpu_interrupt(IRQ_BASE | curr_prio, curr_prio);
        }
        curr_prio--;
    }

    /* Process any commands that may have come through */
    switch (pic.iccr) {
        case PIC_CMD_IRR_R:
            pic.icdr = pic.irr;
            pic.iccr = 0;
            break;
        case PIC_CMD_ISR_R:
            pic.icdr = pic.isr;
            pic.iccr = 0;
            break;
        case PIC_CMD_IMR_R:
            pic.icdr = pic.imr;
            pic.iccr = 0;
            break;
        case PIC_CMD_IMR_W:
            pic.imr = pic.icdr & 0xFF;
            pic.iccr = 0;
            break;
        default:
            break;
    }
}

void raise_irq(int num)
{
    num &= 7;
    if (!IS_BIT_SET(pic.isr, num) && !IS_BIT_SET(pic.imr, num)) {
        SET_BIT(pic.irr, num);
    }
}

void finish_irq(int num)
{
    CLEAR_BIT(pic.isr, num & 7);
}

uint8_t get_irr(void)
{
    return pic.irr;
}

uint8_t get_isr(void)
{
    return pic.isr;
}

uint8_t get_imr(void)
{
    return pic.imr;
}

void set_imr(uint8_t mask)
{
    pic.imr = mask;
}

void set_iccr(lc3word cmd)
{
    pic.iccr = cmd;
}

lc3word get_icdr(void)
{
    return pic.icdr;
}

void set_icdr(lc3word data)
{
    pic.icdr = data;
}
