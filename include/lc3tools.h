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
 *   File: include/lc3tools.h
 * Author: Wes Hampson
 *   Desc: A common library of functions used throughout the lc3tools suite.
 *============================================================================*/

#ifndef __LC3TOOLS_H
#define __LC3TOOLS_H

/*
 * Debug build switch.
 * Uncomment to build lc3tools with debugging info.
 */
#define DEBUG 1

#define TEST_DIR    "../test"

/*
 * File separator character.
 */
#ifdef _WIN32
#define FILE_SEPARATOR   '\\'
#else
#define FILE_SEPARATOR   '/'
#endif


/* ===== Functions defined in src/lib/path.c ===== */

/*
 * Retrieve the file name from a file path string.
 *
 * @param path  a file path
 * @return      the extracted file name
 */
char * get_filename(char *path);

#endif /* __LC3TOOLS_H */
