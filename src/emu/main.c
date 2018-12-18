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
 *   File: src/emu/main.c
 * Author: Wes Hampson
 *   Desc: Entry point for lc3emu.
 *============================================================================*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lc3tools.h>
#include <lc3.h>
#include <cpu.h>
#include <mem.h>
#include <pic.h>

/**
 * POSSIBLE OPTIONS
 * Usage: lc3emu [options] executable
 *   --memory <KiB>   available memory (default 64 KiB, max 64 KiB)
 *   --help
 *   --version
 */

static void usage(const char *prog_name);

int main(int argc, char *argv[])
{
    // FILE *f;
    // lc3word origin;
    // lc3word len;
    // lc3word *prog;
    // int i;

    // if (argc < 2) {
    //     usage(get_filename(argv[0]));
    //     return 1;
    // }

    // f = fopen(argv[1], "rb");
    // if (f == NULL) {
    //     fprintf(stderr, "Error: could not open file.\n");
    //     return 1;
    // }

    // fread(&origin, 2, 1, f);
    // fseek(f, 0x08, SEEK_SET);
    // fread(&len, 2, 1, f);
    // fseek(f, 0x10, SEEK_SET);

    // printf("Origin: 0x%04x\n", origin);
    // printf("Length: 0x%04x\n", len);

    // prog = (lc3word *) malloc(len);

    // /* TODO: check length */
    // fread(prog, 2, len, f);

    // lc3_zero();
    // lc3_writemem(origin, (lc3byte *) prog, len);
    // lc3_writereg(R_PC, origin);
    // lc3_writereg(R_MCR, MCR_CE);
    // lc3_writereg(R_PSR, 0x0702);    /* run program with PL7 */
    // lc3_run();
    // lc3_printregs();

    // free(prog);
    // fclose(f);
    // return 0;

    mem_reset();
    pic_reset();
    cpu_reset();

    /* 0000: BR #-1 (infinite loop at address 0) */
    while (mem_write(0x0000, 0x0FFF, 0xFFFF) == 0) {
        mem_tick();
    }

    /* Interrupt vector for IRQ2 */
    while (mem_write(0x0200 | (0x82 << 1), 0x0400, 0xFFFF) == 0) {
        mem_tick();
    }

    /* Interrupt handler for IRQ2 */
    /* RTI */
    while (mem_write(0x0400, 0x8000, 0xFFFF) == 0) {
        mem_tick();
    }

    /* Main loop */
    for (;;) {
        mem_tick();
        cpu_tick();
    }

    return 0;
}

static void usage(const char *prog_name)
{
    printf("Usage: %s [options] executable\n", prog_name);
    printf("Run '%s --help' for options.\n", prog_name);
}
