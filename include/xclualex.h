#ifndef XC_LLEX_H
#define XC_LLEX_H
#include "arena.h"
#include "xcstring.h"
#include "xcfs.h"
#include "xccommon.h"
#include "xclist.h"

/**
 * @TWOFACE: This token has a different meaning depending on surrounding symbols.
 */

/**
 * If a Lua source file is saved with a UTF-8 BOM (0xEF, 0xBB, 0xBF), 
 * the official Lua 5.4 compiler silently ignores it at the very beginning of the file.
 * Unsure why.
 */
typedef enum {
    // Primitives
    TK_NIL,
    TK_BOOL,
    TK_INT_10,
    TK_INT_16,
    TK_FLOAT,
    TK_STR,
    TK_STR_MUL,
    TK_IDENT,      // Used for variable names (i.e. `local myVar = "hello"` => `myVar` is TK_SYMBOL)
    
    // Arithmetic
    TK_ASSIGN,      // =
    TK_EQ,          // ==
    TK_NEQ,         // ~=
    TK_LT,          // <
    TK_GT,          // >
    TK_LTE,         // <=
    TK_GTE,         // >=
    TK_ADD,         // +
    TK_NEG,         // PARSERS @TWOFACE: `a - b` is different from `-b`
    TK_MUL,         // *
    TK_DIV,         // /
    TK_IDIV,        // //
    TK_MOD,         // %
    TK_EXP,         // ^

    // Reserved literals
    TK_FALSE,
    TK_TRUE,


    // Keywords
    TK_LOCAL,       // local
    
    // Logical keywords
    TK_IF,
    TK_THEN,
    TK_ELSE,
    TK_ELSEIF,
    TK_END,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_IN,

    // Operators
    TK_LEN,         // #myStr => gets the length of `myStr`
    TK_DOT,         // myObject.myFunction() or myObject.myMember. 
                    // Note that while a floating-point may have a dot in it, this token is only emitted on the dot operator (never in a number.)
    TK_CONCAT,      // str1 .. str2 => "%s%s", str1, str2

    // Control
    TK_FUNCTION,
    TK_WHILE,
    TK_FOR,
    TK_DO,
    TK_BREAK,
    TK_REPEAT,
    TK_UNTIL,
    TK_RETURN,
    TK_GOTO,
    TK_VARIADIC,    // ... i.e. local function sum_all(...)

    // Grammar? May not need
    TK_COMMA,
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
    TK_LBRACK,
    TK_RBRACK,
    TK_COLON,
    TK_SEMICOLON,

    // Bitwise
    TK_BWAND,
    TK_BWOR,
    TK_RSHIFT,
    TK_LSHIFT,
    TK_TILDE,       // PARSERS @TWOFACE: a ~ b is XOR (a ^ b), ~b is NOT (~b)
} XcLuaLexType;

typedef struct {
    XcLuaLexType type;
    char* data;
} XcLuaToken;

typedef struct {
    XcLuaToken* tokens;
    size_t n_tokens;
} XcLuaTokens;
typedef struct {
    int x;
} XcLuaLexer;

#ifdef XC_IMPL
#define XC_LUALEX_IMPL
#endif

#define XC_LUALEX_IMPL
#ifdef XC_LUALEX_IMPL
#include <stdio.h>

DECL_LIST(Xcltl, XcLuaToken); // XcLuaTokenList
DECL_LIST(Xcsl, XcStringView); // XcStringView list

XcLuaToken xcll_lex(const XcStringView* s) {
    /**
     * There are several modes we can be lexing in (exclusive):
     * 
     * 0 - Comment Mode
     *      -> --, --[[ (multiline)
     *      -> Do not output any tokens, skip until the end of the comment
     * 1 - Number Mode
     *      -> Identified by beginning with a digit.
     *      -> Value is all characters found until whitespace.
     *      -> Int (16), Float (16.0), or Hex (0x10)
     * 2 - Immediate Mode
     *      -> (comma), LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, DOT (., myObject.myFunc() or myObject.myMember)
     *      -> If we see these, immediately output the corresponding token. May need to look ahead to not skip information
     *      -> For example, `<` => TK_LT, but `<=` => `TK_LTE`. However, `.` at this stage requires no lookahead
     * 3 - String Mode
     *      -> Identified by a beginning '"' or "'" (either are valid and behave identically.)
     *      -> Output the string contained within the quotes
     * 4 - Symbol Mode
     *      -> If the current string exactly matches a defined keyword, output its corresponding token.
     *      -> Must look ahead to ensure we are not skipping information.
     *      -> For example, `local` => `TK_LOCAL`, but `localVar` => TK_IDENT (not this mode)
     * 5 - Identifier Mode
     *      -> If no other mode is satisfied, this must be an identifier. Output all tokens until whitespace.
     */
}

XcLuaTokens xc_lualex_tokenize(arena_t* mem, FILE* f) {
    long flen = xc_fs_filelen(f);
    char* buf = ALLOC_ARRAY(mem, char, flen);
    fread(buf, 1, flen, f);
    XcStringView line, s = xcs(buf);
    Xcltl* list = Xcltl_init();
    
    while (s.count > 0) {
        line = xcs_split(&s, '\n');
        long __llen = line.count + 1; // for \n
        line = xcs_trim(&line);
        printf("|"XCS_FMT"|\n", XCS_Arg(line));
        xcs_chop_left(&s, __llen);

    }

    return (XcLuaTokens) {
        .n_tokens = 0,
        .tokens = NULL,
    };
}

#endif

#endif