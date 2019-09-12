#ifndef __LC3AS_H
#define __LC3AS_H

#include <as/CSourceFile.h>
#include <as/Token.h>

// #ifdef __cplusplus
// extern "C"
// {
// #endif

#define LC3AS_EXE   "lc3as"

#define E_INVALID   -1
#define E_ARGUMENT  -2
#define E_IO        -3
#define E_SYNTAX    -4

// #ifdef __cplusplus
// }
// #endif

void error(const char *fmt, ...);
void error(const CSourceFile& src, const char *fmt, ...);
void error(const CSourceFile& src, int row, int col, const char *fmt, ...);

#endif  /* __LC3AS_H */
