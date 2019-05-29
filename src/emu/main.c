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
 *   File: src/emu/main.c
 * Author: Wes Hampson
 *   Desc: Entry point for lc3emu.
 *============================================================================*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <lc3.h>
#include <cpu.h>
#include <mem.h>
#include <kbd.h>
#include <disp.h>
#include <pic.h>

/* Instruction encodings */
#define _NOP                0
#define _AND(dr,sr1,sr2)    (OP_AND<<12|dr<<9|sr1<<6|sr2&7)
#define _ANDi(dr,sr1,imm)   (OP_AND<<12|dr<<9|sr1<<6|0x20|imm&0x1F)
#define _ADD(dr,sr1,sr2)    (OP_ADD<<12|dr<<9|sr1<<6|sr2&7)
#define _ADDi(dr,sr1,imm)   (OP_ADD<<12|dr<<9|sr1<<6|0x20|imm&0x1F)
#define _NOT(dr,sr)         (OP_XOR<<12|dr<<9|sr<<6|0x3F)
#define _XOR(dr,sr1,sr2)    (OP_XOR<<12|dr<<9|sr1<<6|sr2&7)
#define _XORi(dr,sr1,imm)   (OP_XOR<<12|dr<<9|sr1<<6|0x20|imm&0x1F)
#define _LSHF(dr,sr,imm)    (OP_SHF<<12|dr<<9|sr<<6|imm&0xF)
#define _RSHFL(dr,sr,imm)   (OP_SHF<<12|dr<<9|sr<<6|0x10|imm&0xF)
#define _RSHFA(dr,sr,imm)   (OP_SHF<<12|dr<<9|sr<<6|0x30|imm&0xF)
#define _BRn(pcoff)         (OP_BR<<12|0x800|pcoff&0x1FF)
#define _BRz(pcoff)         (OP_BR<<12|0x400|pcoff&0x1FF)
#define _BRp(pcoff)         (OP_BR<<12|0x200|pcoff&0x1FF)
#define _BRnz(pcoff)        (OP_BR<<12|0xC00|pcoff&0x1FF)
#define _BRnp(pcoff)        (OP_BR<<12|0xA00|pcoff&0x1FF)
#define _BRzp(pcoff)        (OP_BR<<12|0x600|pcoff&0x1FF)
#define _BRnzp(pcoff)       (OP_BR<<12|0xE00|pcoff&0x1FF)
#define _TRAP(vec)          (OP_TRAP<<12|vec)
#define _JSR(off)           (OP_JSR<<12|0x800|off)
#define _JSRR(br)           (OP_JSR<<12|br<<6)
#define _JMP(br)            (OP_JMP<<12|br<<6)
#define _RET()              (OP_JMP<<12|0x1C0)
#define _RTI()              (OP_RTI<<12)
#define _LEA(dr,pcoff)      (OP_LEA<<12|dr<<9|pcoff&0x1FF)
#define _LDB(dr,br,off)     (OP_LDB<<12|dr<<9|br<<6|off&0x3F)
#define _LDW(dr,br,off)     (OP_LDW<<12|dr<<9|br<<6|off&0x3F)
#define _LDI(dr,br,off)     (OP_LDI<<12|dr<<9|br<<6|off&0x3F)
#define _STB(sr,br,off)     (OP_STB<<12|sr<<9|br<<6|off&0x3F)
#define _STW(sr,br,off)     (OP_STW<<12|sr<<9|br<<6|off&0x3F)
#define _STI(sr,br,off)     (OP_STI<<12|sr<<9|br<<6|off&0x3F)
#define _PUSH(sr)           _ADDi(R6, R6, -2),  _STW(sr, R6, 0)
#define _POP(dr)            _LDW(dr, R6, 0),    _ADDi(R6, R6, 2)

/* Register names */
#define R0  (R_0)
#define R1  (R_1)
#define R2  (R_2)
#define R3  (R_3)
#define R4  (R_4)
#define R5  (R_5)
#define R6  (R_6)
#define R7  (R_7)

/**
 * TODO:
 * POSSIBLE COMMAND-LINE OPTIONS
 * Usage: lc3emu [options] executable
 *   --memory <KiB>   available memory (default 64 KiB, max 64 KiB)
 *   --help
 *   --version
 */

static struct termios orig_termios;

static inline void dev_tick(void);

static void write_word(lc3word addr, lc3word data);
static void fill_mem(lc3word addr, const lc3word *data, int n);

static void usage(const char *prog_name);

static void set_nonblock(void);
static void reset_terminal(void);

#define OS_ADDR         0x0400
#define DISP_ISR        0x0500
#define KBD_ISR         0x0600

const lc3word os_code[] =
{
    /* == Operating System Code == */
    /* Disable interrupts from the display device, then spin forever. */

    /* Code */
    _LEA(R0, 5),
    _LDW(R2, R0, 3),    /* r2 = mask                            */
    _LDW(R3, R0, 2),    /* r3 = cmd                             */
    _STI(R2, R0, 1),    /* *icdr_addr = r2                      */
    _STI(R3, R0, 0),    /* *iccr_addr = r3                      */
    _BRnzp(-1),

    /* Data */
    A_ICCR,             /* iccr_addr */
    A_ICDR,             /* icdr_addr */
    PIC_CMD_IMR_W,      /* cmd: write PIC mask register         */
    (1 << DISP_IRQ)     /* mask: display device IRQ bit         */
};

const lc3word isr3_code[] =
{
    /* == Display Device ISR Code == */
    /* Continually write NUL. This interrupt will keep firing as long as the
       display device is ready to take a character. To prevent it from eating
       up CPU cycles, keep writing NUL until we get a chance to mask interrupts
       from the display device.
    */

    /* Code */
    _PUSH(R0),
    _PUSH(R1),
    _LEA(R0, 7),
    _LDW(R1, R0, 1),    /* r1 = nul                             */
    _STI(R1, R0, 0),    /* *ddr_addr = r1                       */
    _POP(R1),
    _POP(R0),
    _RTI(),

    /* Data */
    A_DDR,              /* ddr_addr                             */
    0x0000,             /* nul                                  */
};

const lc3word isr4_code[] =
{
    /* == Keyboard ISR Code == */
    /* Clears the 'ready' bit in KBSR, then displays the character typed by
       writing the value of KBDR to DDR. */

    /* Code */
    _PUSH(R0),
    _PUSH(R1),
    _PUSH(R2),
    _LEA(R0, 13),
    _LDI(R1, R0, 0),        /* kbsr = *kbsr_addr                */
    _LDW(R2, R0, 1),        /* mask = kbsr_mask                 */
    _AND(R1, R1, R2),       /* kbsr &= mask                     */
    _STI(R1, R0, 0),        /* *kbsr_addr = kbsr                */
    _LDI(R1, R0, 2),        /* char c = *kbdr_addr              */
    _STI(R1, R0, 3),        /* *ddr_addr = c;                   */
    _POP(R2),
    _POP(R1),
    _POP(R0),
    _RTI(),

    /* Data */
    A_KBSR,                 /* kbsr_addr                        */
    0x7FFF,                 /* kbsr_mask                        */
    A_KBDR,                 /* kbdr_addr                        */
    A_DDR                   /* ddr_addr                         */
};

int main(int argc, char *argv[])
{
    /* Put terminal into raw mode */
    set_nonblock();

    /* Reset machine state */
    mem_reset();
    kbd_reset();
    disp_reset();
    pic_reset();
    cpu_reset();

    /* Initialize IVT */
    write_word(A_IVT | ((IRQ_BASE | DISP_IRQ) << 1), DISP_ISR);
    write_word(A_IVT | ((IRQ_BASE | KBD_IRQ) << 1), KBD_ISR);

    /* Write OS and ISR code to RAM */
    fill_mem(OS_ADDR, os_code, sizeof(os_code) / sizeof(lc3word));
    fill_mem(DISP_ISR, isr3_code, sizeof(isr3_code) / sizeof(lc3word));
    fill_mem(KBD_ISR, isr4_code, sizeof(isr4_code) / sizeof(lc3word));

    /* Go! */
    while (get_mcr() & MCR_CE) {
        dev_tick();
        cpu_tick();
    }

    return 0;
}

static inline void dev_tick(void)
{
    mem_tick();
    kbd_tick();
    disp_tick();
    pic_tick();
}

static void write_word(lc3word addr, lc3word data)
{
    mem_write_nodelay(addr, data, 0xFFFF);
}

static void fill_mem(lc3word addr, const lc3word *data, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        mem_write_nodelay(addr + (i << 1), data[i], 0xFFFF);
    }
}

static void usage(const char *prog_name)
{
    printf("Usage: %s [options] executable\n", prog_name);
    printf("Run '%s --help' for options.\n", prog_name);
}

static void set_nonblock(void)
{
    struct termios term;

    tcgetattr(STDIN_FILENO, &orig_termios);
    memcpy(&term, &orig_termios, sizeof(struct termios));

    atexit(reset_terminal);
    cfmakeraw(&term);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static void reset_terminal(void)
{
    cpu_dumpregs();
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}
