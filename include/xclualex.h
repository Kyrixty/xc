#ifndef XC_LLEX_H
#define XC_LLEX_H
#include "arena.h"
#include "xcstring.h"
#include "xcfs.h"
#include "xccommon.h"
#include "xclist.h"
#include "xcutils.h"

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
    
    // File-related
    TK_EOF,
} XcLuaLexType;

typedef struct {
    XcLuaLexType type;
    XcStringView view;
    union {
        int     num;
        double  dec;
        char*   str;
    } val;
    size_t line;
    size_t col;
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

typedef enum {
    XCLL_MODE_NONE,
    XCLL_MODE_ERROR,
    XCLL_MODE_COMMENT,
    XCLL_MODE_COMMENT_MULTI,
    XCLL_MODE_NUM,
    XCLL_MODE_IMM,
    XCLL_MODE_STR,
    XCLL_MODE_STR_MULTI,
    XCLL_MODE_SYMBOL,
    XCLL_MODE_IDENT,
} XcLuaLexMode;

typedef enum {
    XCLL_STATUS_WAITING,
    XCLL_STATUS_INPROGRESS,
} XcLuaLexStatus;

typedef struct {
    XcLuaLexMode mode;
    XcLuaLexStatus status;
    bool didWrite;
    XcStringView currView;
    size_t colno;
    size_t lnno;
} XcLuaLexContext;

static XcLuaLexContext xcllctx;

void xcll_lex_comment(XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    // This is a single-line comment. Clear `s` and output nothing.
    XcStringView comment = xcs_split(s, '\n');
    xcs_chop_left(s, comment.count);
    ctx->didWrite = false;
    ctx->mode = XCLL_MODE_NONE;
    ctx->status = XCLL_STATUS_WAITING;
}

void xcll_lex_comment_multiline(XcStringView* s, XcLuaToken* tokenl, XcLuaLexContext* ctx) {
    size_t idx = xcs_index_cstr(s, "]]");
    if (idx == XCS_NOT_FOUND) {
        XCS_LEXER_ERROR("Cannot find end of multiline comment. Expected ']]' but got <EOF>.")
    }
    ctx->didWrite = false;
    ctx->mode = XCLL_MODE_NONE;
    ctx->status = XCLL_STATUS_WAITING;
    xcs_chop_left(s, idx + xc_strlen("]]"));
}

void xcll_lex_str(XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    // This is a single-line string. Parse until the end of the string and output a TK_STR
    char prev = -1, first = xcs_at(s, 0);
    int str_end_idx = -1;
    bool escaping = false;
    xcs_chop_left(s, 1); // remove leading quotation mark

    for (size_t i = 0; i < s->count; i++) {
        char c = xcs_at(s, i);
        if (!escaping && prev == '\\') {
            escaping = true;
        }
        if (c == first && !escaping) {
            str_end_idx = i;
            break;
        }
        if (escaping) {
            escaping = false;
        }
        prev = c;
    }

    if (s->count <= 1 || str_end_idx == -1) {
        XCS_LEXER_ERROR("Cannot find end of string. '' Expected '%c', got '\\n'\n", first);
    }

    token->type = TK_STR;
    token->view = *s;
    token->view.count = MAX(str_end_idx, 0);
    xcs_chop_left(s, token->view.count + 1); // + 1 for trailing quotation mark
    ctx->didWrite = true;
    ctx->mode = XCLL_MODE_NONE;
    ctx->status = XCLL_STATUS_WAITING;
}

void xcll_lex_str_multiline(XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    xcs_chop_left(s, 2);
    // Lua ignores the first newline after [[ if [[ is immediately followed by said newline.
    /**
     * i.e.
     * [[
     * String
     * ]]
     *  => "String\n"
     * but,
     * [[String
     * ]]
     *  => "String\n\n" (trailing \n after "Strin\n" included)
     */
    *s = xcs_trim_left(s);
    size_t idx = xcs_index_cstr(s, "]]");
    if (idx == XCS_NOT_FOUND) {
        XCS_LEXER_ERROR("Cannot find end of multi-line string. Expected ']]' but got <EOF>.");
    }
    size_t len = idx == 0 ? 0 : idx - 1; // 1 char before [[
    token->view = *s;
    token->view.count = len;
    token->type = TK_STR;
    xcs_chop_left(s, idx + 2); // chop off str, including trailing ]]
    ctx->didWrite = true;
    ctx->mode = XCLL_MODE_NONE;
    ctx->status = XCLL_STATUS_WAITING;
}

void xcll_lex_num(arena_t* mem, XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    XcStringView numView = xcs_collect_until(s, isspace);
    size_t dotIdx = xcs_index_cstr(&numView, ".");
    size_t hexIdx = xcs_index_cstr(&numView, "0x");
    if (dotIdx != XCS_NOT_FOUND && hexIdx != XCS_NOT_FOUND) {
        if (dotIdx < hexIdx) {
            XCS_LEXER_ERROR("Unexpected token '0x' while parsing float.");
        } else {
            XCS_LEXER_ERROR("Unexpected token '.' while parsing hex number.");
        }
    }
    
    char* numBuf = ALLOC_ARRAY(mem, char, numView.count + 1);
    numBuf[numView.count] = 0;
    xc_memcpy(numBuf, numView.data, numView.count);
    if (dotIdx == hexIdx) {
        // Only possible if both are XCS_NOT_FOUND
        size_t nParsed = 0;
        int v = xc_parse_int(numView.data, 10, &nParsed);
        if (nParsed != numView.count) {
            XCS_LEXER_ERROR("Error while parsing int: '"XCS_FMT"' is invalid (parsed %llu characters)", XCS_Arg(numView), nParsed);
        }
        token->val.num = v;
        token->type = TK_INT_10;
    } else if (dotIdx != XCS_NOT_FOUND) {
        // There is no dedicated spot for it. Instead, we detect parsing errors with strtod
        char* end = NULL;
        double v = strtod(numView.data, &end);
        if ((end - numView.data) != numView.count) {
            XCS_LEXER_ERROR("Error while parsing float: '"XCS_FMT"' is invalid (parsed %llu chars).", XCS_Arg(numView), numView.count - (end - numView.data));
        }
        token->val.dec = v;
        token->type = TK_FLOAT;
    } else if (hexIdx != XCS_NOT_FOUND) {
        // Must be of the form '0xffac...'. If x is found later, it is invalid.
        if (hexIdx != 0) {
            XCS_LEXER_ERROR("Error while parsing hex number: '0x' found at index %llu but expected at index 0.", hexIdx);
        }
        //HACK
        size_t nParsed = 0;
        int num = xc_parse_int(numView.data + 2, 16, &nParsed);
        if (nParsed != numView.count - 2) {
            XCS_LEXER_ERROR("Error while parsing hex number: '"XCS_FMT"' is invalid (parsed %llu chars).", XCS_Arg(numView), nParsed);
        }
        token->type = TK_INT_16;
        token->val.num = num;
    } else {
        XCS_LEXER_ERROR("<xcll_lex_num>: No dot or x index found while parsing num.");
    }

    // TODO: ctx modify
    // Fortunately, in all cases the view parsing is the same
    token->view = numView;
    xcs_chop_left(s, numView.count);
    ctx->didWrite = true;
    ctx->mode = XCLL_MODE_NONE;
}

/**
 * Lexes an `XcStringView`, trimming off all characters consumed in the process.
 * Lexes via maximal-munch.
 * 
 * @param   s   The string view to be lexed. Will be modified after calling to
 * remove any characters consumed. Callers should repeat calling this function until `s`
 * is empty.
 * @param   ctx The current lexer context
 * @param[out]  token The destination token to be modified should a token be outputted.
 * Callers can know this value was modified if `ctx->didWrite == true`.
 */
void xcll_lex(arena_t* mem, XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
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
     *      -> (comma), LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, DOT (., myObject.myFunc() or myObject.myMember), etc.
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
     *      -> Note that this identifier outputted may not *actually* be a valid identifier, but the parser/analyzer will handle that.
     * 
     * xcll_lex will first determine which mode to lex in and on the NEXT CALL use this mode to lex correctly.
     */

    // TODO: Need a trie for keyword lookups OR a hash table via gperf
    // TODO: need line + col numbers
    // TODO: each mode should have it's own handler that just gets called from here. We can just interpret the context here and route to the correct goto label.
    // IDEA: just pass lexeme into lex functions in case we want to verify outputs or something along those lines.
    // NOTE: Currently in process of migrating the lexer to not just handle single lines, since a lot
    // of things in Lua require multiple lines (comments, strings, tables, etc.)
    // As a result, xcs_consume_all should NOT be used ANYWHERE on `s` in this function!
    XcLuaToken lexeme = {0};
    *s = xcs_trim_left(s); // Remove leading whitespace
    if (ctx->mode == XCLL_MODE_NONE) {
        // We have just started lexing a new token.
        // Need to determine the mode, set it, then pass control back to the caller.

        /**STRICT:
         * The mode selector should not modify input. Even if it seems unimportant, do not alter
         * input past what `s` is at this point. That is the job of xcll_lex_* functions.
         */
        if (xcs_empty(s)) {
            token->view = *s;
            token->type = TK_EOF;
            ctx->didWrite = true;
            goto J_LLEX_WAITING;
        }
        char first = xcs_at(s, 0);
        if (xcs_startswith_cstr(s, "--")) {
            ctx->mode = xcs_startswith_cstr(s, "--[[") ? XCLL_MODE_COMMENT_MULTI : XCLL_MODE_COMMENT;
        }
        else if (xcs_startswith_cstr(s, "[[")) {
            ctx->mode = XCLL_MODE_STR_MULTI;
        }
        else if (first == '"' || first == '\'') {
            ctx->mode = XCLL_MODE_STR;
        }
        else if ('0' <= first && first <= '9') {
            ctx->mode = XCLL_MODE_NUM;
        }
        goto J_LLEX_INPROGRESS;
    }

    if (ctx->mode == XCLL_MODE_COMMENT_MULTI) {
        // We are currently processing a multi-line comment block.
        // This block ends directly after we find ]]
        // Skip token processing until this occurs.
        xcll_lex_comment_multiline(s, token, ctx);
        goto J_LLEX_WAITING;
    }

    if (ctx->mode == XCLL_MODE_COMMENT) {
        // This is a single-line comment. Clear `s` and output nothing.
        xcll_lex_comment(s, token, ctx);
        goto J_LLEX_WAITING;
    }

    if (ctx->mode == XCLL_MODE_NUM) {
        xcll_lex_num(mem, s, token, ctx);
        goto J_LLEX_WAITING;
    }

    if (ctx->mode == XCLL_MODE_STR) {
        xcll_lex_str(s, token, ctx);
        goto J_LLEX_WAITING;
    }

    if (ctx->mode == XCLL_MODE_STR_MULTI) {
        xcll_lex_str_multiline(s, token, ctx);
        goto J_LLEX_WAITING;
    }

J_LLEX_INPROGRESS:
    ctx->status = XCLL_STATUS_INPROGRESS;
    ctx->didWrite = false;
    return;

J_LLEX_WAITING:
    ctx->status = XCLL_STATUS_WAITING;

// J_LLEX_DIDWRITE:
    if (ctx->didWrite) {
        // See the lexeme idea above
        // *token = lexeme;
    }
}

XcLuaTokens xc_lualex_tokenize(arena_t* mem, FILE* f) {
    long flen = xc_fs_filelen(f);
    char* buf = ALLOC_ARRAY(mem, char, flen + 1);
    fread(buf, 1, flen, f);
    buf[flen] = 0;
    XcStringView s = xcs(buf);
    Xcltl* list = Xcltl_init();
    size_t lnno = 0, colno = 0;
    XcLuaToken token = {0};
    
    while (!xcs_empty(&s)) {
        xcll_lex(mem, &s, &token, &xcllctx);
        if (xcllctx.didWrite) {
            Xcltl_append(list, token);
            printf("|"XCS_FMT"|\n", XCS_Arg(token.view));
        }
        printf("%p %llu\n", s.data, s.count);
    }
    xcll_lex(mem, &s, &token, &xcllctx);  // EOF
    Xcltl_append(list, token);

    return (XcLuaTokens) {
        .n_tokens = 0,
        .tokens = NULL,
    };
}

#endif

#endif