#ifndef XC_ERROR_H
#define XC_ERROR_H

#include <stdlib.h>
#include <stdio.h>

// Need to figure out a nice error report format. For now, just going to wing it.

#define XCS_ERR_CODE_GENERIC 1          // Generic error code
#define XCS_ERR_CODE_LEXER 255          // Lexer error code
#define XCS_ERR_CODE_PARSER 254         // Parser error code
#define __XCS_ERROR(CODE, FMT, ...) \
    fprintf(stderr, "\033[31mXCS: \033[0m"FMT, __VA_ARGS__);    \
    exit(CODE);

#define XCS_ERROR(FMT, ...)         __XCS_ERROR(XCS_ERR_CODE_GENERIC, FMT, __VA_ARGS__);
#define XCS_LEXER_ERROR(FMT, ...)   __XCS_ERROR(XCS_ERR_CODE_LEXER, "[Lexer]\t"FMT, __VA_ARGS__);
#define XCS_PARSER_ERROR(FMT, ...)  __XCS_ERROR(XCS_ERR_CODE_PARSER, "[Parser]\t"FMT, __VA_ARGS__);

#endif