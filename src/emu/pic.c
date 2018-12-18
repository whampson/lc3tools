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

#include <pic.h>

static struct lc3pic pic;

void pic_reset(void)
{
    pic.irq_mask = 0;
}

void raise_irq(int num)
{
    pic.irq_mask |= (1 << (num & 7));
}

void clear_irq(int num)
{
    pic.irq_mask &= ~(1 << (num & 7));
}

uint8_t get_irq_mask(void)
{
    return pic.irq_mask;
}
