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

#include <lc3.h>

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

    // if (argc < 2) {
    //     usage(get_filename(argv[0]));
    //     return 1;
    // }

    /* TODO: argument parsing */

    lc3word data[64];
    lc3word buf[64];
    int i;

    lc3_reset();
    memset(data, 0, 64);
    memset(buf, 0, 64);

    data[0] = 0x0AAA;
    data[1] = 0x1AAA;
    data[2] = 0x2AAA;
    data[3] = 0x3AAA;
    data[4] = 0x4AAA;
    data[5] = 0x5AAA;
    data[6] = 0x6AAA;
    data[7] = 0x7AAA;
    data[8] = 0x8AAA;
    data[9] = 0x9AAA;
    data[10] = 0xAAAA;
    data[11] = 0xBAAA;
    data[12] = 0xCAAA;
    data[13] = 0xDAAA;
    data[14] = 0xEAAA;
    data[15] = 0xFAAA;
    data[16] = 0x0FFF;
    lc3_writemem(0, (lc3byte *) data, sizeof(data) << 1);
    lc3_execute(0);

    // lc3_readmem((lc3byte *) buf, 0, 6);

    // for (i = 0; i < 3; i++) {
    //     printf("%04x: 0x%04x\n", i * 2, buf[i]);
    // }

    lc3_printregs();

    return 0;
}

static void usage(const char *prog_name)
{
    printf("Usage: %s [options] executable\n", prog_name);
    printf("Run '%s --help' for options.\n", prog_name);
}
