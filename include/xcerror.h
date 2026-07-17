#ifndef XC_ERROR_H
#define XC_ERROR_H

#include <stdlib.h>
#include <stdio.h>

// Need to figure out a nice error report format. For now, just going to wing it.

#define XCS_ERR_CODE_GENERIC 1          // Generic error code
#define XCS_ERR_CODE_LEXER 255          // Lexer error code
#define XCS_ERR_CODE_PARSER 254         // Parser error code
#define __XCS_ERROR(CODE, FMT, ...) \
    fprintf(stderr, "\033[31mXCS: \033[0m (line: %d) "FMT"\n", __LINE__ __VA_OPT__(,) __VA_ARGS__);    \
    exit(CODE);

#define XCS_ERROR(FMT, ...)                         __XCS_ERROR(XCS_ERR_CODE_GENERIC, FMT __VA_OPT__(,) __VA_ARGS__);
#define _XCS_LEXER_ERROR(FMT, VIEWFMT, LINE, COL, VIEW, ...)    __XCS_ERROR(XCS_ERR_CODE_LEXER, "[Lexer]"VIEWFMT FMT, LINE, COL, VIEW __VA_OPT__(,) __VA_ARGS__);
#define XCS_PARSER_ERROR(FMT, VIEWFMT, LINE, COL, VIEW, ...)   __XCS_ERROR(XCS_ERR_CODE_PARSER, "[Parser]"VIEWFMT FMT, LINE, COL, VIEW __VA_OPT__(,) __VA_ARGS__);

#endif