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
 *   File: src/emu/kbd.c
 * Author: Wes Hampson
 *   Desc: Keyboard input device driver.
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include <kbd.h>
#include <pic.h>

#define RD()        (kbd.kbsr & KBSR_RD)
#define SET_RD(x)   (kbd.kbsr = (x)?(kbd.kbsr|KBSR_RD):(kbd.kbsr&~KBSR_RD))

#define IE()        (kbd.kbsr & KBSR_IE)
#define SET_IE(x)   (kbd.kbsr = (x)?(kbd.kbsr|KBSR_IE):(kbd.kbsr&~KBSR_IE))

static struct lc3kbd kbd;

static int kbhit(void);
static int getch(void);

void kbd_reset(void)
{
    memset(&kbd, 0, sizeof(struct lc3kbd));
    SET_IE(1);
}

void kbd_tick(void)
{
    unsigned char c;

    if (kbhit()) {
        c = getch();
        if (c == 3) {
            printf("CTRL+C pressed!\r\n");
            exit(127);
        }
        kbd.kbdr = c;
        SET_RD(1);
    }

    if (RD() && IE()) {
        raise_irq(KBD_IRQ);
    }
}

lc3word get_kbsr(void)
{
    return kbd.kbsr;
}

void set_kbsr(lc3word value)
{
    kbd.kbsr = value;
}

lc3word get_kbdr(void)
{
    return kbd.kbdr;
}

void set_kbdr(lc3word value)
{
    kbd.kbdr = value;
}

static int kbhit(void)
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    return select(1, &fds, NULL, NULL, &tv);
}

static int getch(void)
{
    int r;
    unsigned char c;

    if ((r = read(STDIN_FILENO, &c, sizeof(unsigned char))) < 0) {
        return r;
    }

    return c;
}
