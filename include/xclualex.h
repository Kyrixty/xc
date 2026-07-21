#ifndef XC_LLEX_H
#define XC_LLEX_H
#include "arena.h"
#include "xcstring.h"
#include "xcfs.h"
#include "xccommon.h"
#include "xclist.h"
#include "xcutils.h"
#include "xcfilters.h"
#include "luakeywords.h"

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
    TK_PLUS,        // +
    TK_MINUS,       // PARSERS @TWOFACE: `a - b` is different from `-b`
    TK_MUL,         // *
    TK_DIV,         // /
    TK_IDIV,        // //
    TK_MOD,         // %
    TK_EXP,         // ^
    TK_SQUIGGLE,    // ~  || PARSERS @TWOFACE: a ~ b is XOR (a ^ b), ~b is NOT (~b)

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
    
    // File-related
    TK_EOF,

    // Debug
    TK_ERROR,       // Generally found when wordToTypeMap fails
} XcLuaLexType;

static XcStringView* lineSrcMap;

char* tkTypeStrMap[] = {
    // Primitives
    [TK_NIL] = "TK_NIL",
    [TK_BOOL] = "TK_BOOL",
    [TK_INT_10] = "TK_INT_10",
    [TK_INT_16] = "TK_INT_16",
    [TK_FLOAT] = "TK_FLOAT",
    [TK_STR] = "TK_STR",
    [TK_IDENT] = "TK_IDENT",
    
    // Arithmetic
    [TK_ASSIGN] = "TK_ASSIGN",
    [TK_EQ] = "TK_EQ",
    [TK_NEQ] = "TK_NEQ",
    [TK_LT] = "TK_LT",
    [TK_GT] = "TK_GT",
    [TK_LTE] = "TK_LTE",
    [TK_GTE] = "TK_GTE",
    [TK_PLUS] = "TK_PLUS",
    [TK_MINUS] = "TK_MINUS",
    [TK_MUL] = "TK_MUL",
    [TK_DIV] = "TK_DIV",
    [TK_IDIV] = "TK_IDIV",
    [TK_MOD] = "TK_MOD",
    [TK_EXP] = "TK_EXP",

    // Reserved literals
    [TK_FALSE] = "TK_FALSE",
    [TK_TRUE] = "TK_TRUE",

    // Keywords
    [TK_LOCAL] = "TK_LOCAL",
    
    // Logical keywords
    [TK_IF] = "TK_IF",
    [TK_THEN] = "TK_THEN",
    [TK_ELSE] = "TK_ELSE",
    [TK_ELSEIF] = "TK_ELSEIF",
    [TK_END] = "TK_END",
    [TK_AND] = "TK_AND",
    [TK_OR] = "TK_OR",
    [TK_NOT] = "TK_NOT",
    [TK_IN] = "TK_IN",

    // Operators
    [TK_LEN] = "TK_LEN",
    [TK_DOT] = "TK_DOT",
    [TK_CONCAT] = "TK_CONCAT",

    // Control
    [TK_FUNCTION] = "TK_FUNCTION",
    [TK_WHILE] = "TK_WHILE",
    [TK_FOR] = "TK_FOR",
    [TK_DO] = "TK_DO",
    [TK_BREAK] = "TK_BREAK",
    [TK_REPEAT] = "TK_REPEAT",
    [TK_UNTIL] = "TK_UNTIL",
    [TK_RETURN] = "TK_RETURN",
    [TK_GOTO] = "TK_GOTO",
    [TK_VARIADIC] = "TK_VARIADIC",

    // Grammar
    [TK_COMMA] = "TK_COMMA",
    [TK_LPAREN] = "TK_LPAREN",
    [TK_RPAREN] = "TK_RPAREN",
    [TK_LBRACE] = "TK_LBRACE",
    [TK_RBRACE] = "TK_RBRACE",
    [TK_LBRACK] = "TK_LBRACK",
    [TK_RBRACK] = "TK_RBRACK",
    [TK_COLON] = "TK_COLON",
    [TK_SEMICOLON] = "TK_SEMICOLON",

    // Bitwise
    [TK_BWAND] = "TK_BWAND",
    [TK_BWOR] = "TK_BWOR",
    [TK_RSHIFT] = "TK_RSHIFT",
    [TK_LSHIFT] = "TK_LSHIFT",
    [TK_SQUIGGLE] = "TK_SQUIGGLE",
    
    // File-related
    [TK_EOF] = "TK_EOF"
};

// Generated from wordlist in luakeywords.h, be careful when modifying/regenerating
XcLuaLexType wordToTypeMap[] = {
    TK_ERROR, TK_ERROR,
    TK_IN,
    TK_NIL,
    TK_ERROR,
    TK_LOCAL,
    TK_RETURN,
    TK_IF,
    TK_FOR,
    TK_ERROR,
    TK_WHILE,
    TK_ERROR,
    TK_OR,
    TK_FUNCTION,
    TK_ELSE,
    TK_FALSE,
    TK_ELSEIF,
    TK_ERROR,
    TK_NOT,
    TK_THEN,
    TK_UNTIL,
    TK_REPEAT,
    TK_ERROR,
    TK_END,
    TK_TRUE,
    TK_BREAK,
    TK_ERROR,
    TK_DO,
    TK_AND,
    TK_GOTO,
};

#define LT '<'
#define GT '>'
#define BWAND '&'
#define BWOR '|'
#define EQ '='
#define PLUS '+'
#define MINUS '-'
#define MUL '*'
#define DIV '/'
#define NEG '-'
#define MOD '%'
#define EXP '^'
#define LEN '#'
#define DOT '.'
#define COMMA ','
#define LPAREN '('
#define RPAREN ')'
#define LBRACE '{'
#define RBRACE '}'
#define LBRACK '['
#define RBRACK ']'
#define COLON ':'
#define SEMICOLON ';'
#define SQUIGGLE '~'

XcLuaLexType charToTypeMap[256] = {
    // Arithmetic operators
    [PLUS]      = TK_PLUS,
    [MINUS]     = TK_MINUS,   // Maps to NEG; parser can contextually treat as binary subtraction
    [MUL]       = TK_MUL,
    [DIV]       = TK_DIV,     // Parser lookahead checks for second '/' to upgrade to TK_IDIV
    [MOD]       = TK_MOD,
    [EXP]       = TK_EXP,

    // Assignment & Comparisons
    [EQ]        = TK_ASSIGN,  // Default single '='. Parser lookahead upgrades to TK_EQ on '=='
    [LT]        = TK_LT,      // Default '<'. Parser lookahead upgrades to TK_LTE on '<=' or TK_LSHIFT on '<<'
    [GT]        = TK_GT,      // Default '>'. Parser lookahead upgrades to TK_GTE on '>=' or TK_RSHIFT on '>>'

    // String & Length operators
    [LEN]       = TK_LEN,
    [DOT]       = TK_DOT,     // Default '.'. Parser lookahead checks for '..' (TK_CONCAT) or '...' (TK_VARIADIC)

    // Grammar & Structure delimiters
    [COMMA]     = TK_COMMA,
    [LPAREN]    = TK_LPAREN,
    [RPAREN]    = TK_RPAREN,
    [LBRACE]    = TK_LBRACE,
    [RBRACE]    = TK_RBRACE,
    [LBRACK]    = TK_LBRACK,
    [RBRACK]    = TK_RBRACK,
    [COLON]     = TK_COLON,
    [SEMICOLON] = TK_SEMICOLON,

    // Bitwise operators (Lua 5.3+)
    [BWAND]     = TK_BWAND,
    [BWOR]      = TK_BWOR,
    [SQUIGGLE]  = TK_SQUIGGLE,   // Default '~'. Parser lookahead upgrades to TK_NEQ if followed by '='
};


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
    const char* fileName;
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
    XCLL_MODE_STR,
    XCLL_MODE_STR_MULTI,
    XCLL_MODE_OPERATOR,
    XCLL_MODE_KEYWORD,
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

#define XCS_LEXER_ERROR(fmt, token, ...) _XCS_LEXER_ERROR(fmt, "\n%s@%llu:%llu\n\t"XCS_FMT"\n", \
    (token)->fileName, (token)->line, (token)->col, XCS_Arg(lineSrcMap[(token)->line - 1]) __VA_OPT__(,) __VA_ARGS__)
static const XcLuaToken NULL_XCLL_TOKEN = {0};

void xcll_lex_comment(XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    // This is a single-line comment. Clear `s` and output nothing.
    XcStringView comment = xcs_split(s, '\n');
    xcs_chop_left(s, comment.count);
    ctx->didWrite = false;
    ctx->mode = XCLL_MODE_NONE;
    ctx->status = XCLL_STATUS_WAITING;
}

void xcll_lex_comment_multiline(XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    size_t idx = xcs_index_cstr(s, "]]", 0);
    if (idx == XCS_NOT_FOUND) {
        XCS_LEXER_ERROR("Cannot find end of multiline comment. Expected ']]' but got <EOF>.", token);
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
        XCS_LEXER_ERROR("Cannot find end of string. Expected '%c', got '\\n'\n", token, first);
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
    size_t idx = xcs_index_cstr(s, "]]", 0);
    if (idx == XCS_NOT_FOUND) {
        XCS_LEXER_ERROR("Cannot find end of multi-line string. Expected ']]' but got <EOF>.", token);
    }
    token->view = *s;
    token->view.count = idx; // 1 char before [[
    token->type = TK_STR;
    xcs_chop_left(s, idx + 2); // chop off str, including trailing ]]
    ctx->didWrite = true;
    ctx->mode = XCLL_MODE_NONE;
    ctx->status = XCLL_STATUS_WAITING;
}



void xcll_lex_num(arena_t* mem, XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    // BUG: this doesn't actually validate that the input is valid.
    // For example, 5.123.122.4444 will be lexed fully even though this should error.
    // xcs_collect probably isn't enough here, we probably need to write the number collector by hand
    XcStringView numView = xcs_collect(s, xcf_is_alnum_or_dot);
    size_t dotIdx = xcs_index_cstr(&numView, ".", 0);
    size_t hexIdx = xcs_index_cstr(&numView, "0x", 0);
    if (dotIdx != XCS_NOT_FOUND && hexIdx != XCS_NOT_FOUND) {
        if (dotIdx < hexIdx) {
            XCS_LEXER_ERROR("Unexpected token '0x' while parsing float.", token);
        } else {
            XCS_LEXER_ERROR("Unexpected token '.' while parsing hex number.", token);
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
            XCS_LEXER_ERROR("Error while parsing int: '"XCS_FMT"' is invalid (parsed %llu characters)", token, XCS_Arg(numView), nParsed);
        }
        token->val.num = v;
        token->type = TK_INT_10;
    } else if (dotIdx != XCS_NOT_FOUND) {
        // There is no dedicated spot for it. Instead, we detect parsing errors with strtod
        char* end = NULL;
        double v = strtod(numView.data, &end);
        if ((end - numView.data) != numView.count) {
            XCS_LEXER_ERROR("Error while parsing float: '"XCS_FMT"' is invalid (parsed %llu chars).", token, XCS_Arg(numView), numView.count - (end - numView.data));
        }
        token->val.dec = v;
        token->type = TK_FLOAT;
    } else if (hexIdx != XCS_NOT_FOUND) {
        // Must be of the form '0xffac...'. If x is found later, it is invalid.
        if (hexIdx != 0) {
            XCS_LEXER_ERROR("Error while parsing hex number: '0x' found at index %llu but expected at index 0.", token, hexIdx);
        }
        //HACK
        size_t nParsed = 0;
        int num = xc_parse_int(numView.data + 2, 16, &nParsed);
        if (nParsed != numView.count - 2) {
            XCS_LEXER_ERROR("Error while parsing hex number: '"XCS_FMT"' is invalid (parsed %llu chars).", token, XCS_Arg(numView), nParsed);
        }
        token->type = TK_INT_16;
        token->val.num = num;
    } else {
        XCS_LEXER_ERROR("<xcll_lex_num>: No dot or x index found while parsing num. Please report this to the xc developers.", token);
    }

    // Fortunately, in all cases the view parsing is the same
    token->view = numView;
    xcs_chop_left(s, numView.count);
    ctx->didWrite = true;
    ctx->mode = XCLL_MODE_NONE;
}

void xcll_lex_operator(XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    char first = xcs_at(s, 0), second;
    XcStringView view = xcs_substring(s, 0, 1);
    // one-char operators
    switch (first) {
        case PLUS:
        case MINUS:
        case MUL:
        case MOD:
        case EXP:
        case LEN:
        case COMMA:
        case LPAREN:
        case RPAREN:
        case LBRACE:
        case RBRACE:
        case LBRACK:
        case RBRACK:
        case COLON:
        case SEMICOLON: {
            token->type = charToTypeMap[first];
            token->view = view;
            goto XCLL_LLEX_OP_RET;
        }
    }

    bool isSingle = s->count == 1;
    second = xcs_at(s, isSingle ? 0 : 1);
    switch (first) {
        case DOT: {
            if (!isSingle && first == second && s->count >= 3 && first == xcs_at(s, 2)) {
                token->type = TK_VARIADIC;
                view = xcs_substring(s, 0, 3);
            } else if (!isSingle && first == second) {
                token->type = TK_CONCAT;
                view = xcs_substring(s, 0, 2);
            } else {
                token->type = TK_DOT;
            }
        }; break;
        case LT: {
            if (!isSingle && first == second) {
                token->type = TK_LSHIFT;
                view = xcs_substring(s, 0, 2);
            } else if (!isSingle && second == EQ) {
                token->type = TK_LTE;
                view = xcs_substring(s, 0, 2);
            } else {
                token->type = TK_LT;
            }
        }; break;
        case GT: {
            if (!isSingle && first == second) {
                token->type = TK_RSHIFT;
                view = xcs_substring(s, 0, 2);
            } else if (!isSingle && second == EQ) {
                token->type = TK_GTE;
                view = xcs_substring(s, 0, 2);
            } else {
                token->type = TK_GT;
            }
        }; break;
        case BWAND: {
            if (!isSingle && first == second) {
                token->type = TK_AND;
                view = xcs_substring(s, 0, 2);
            } else {
                token->type = TK_BWAND;
            }
        }; break;
        case BWOR: {
            if (!isSingle && first == second) {
                token->type = TK_OR;
                view = xcs_substring(s, 0, 2);
            } else {
                token->type = TK_BWOR;
            }
        }; break;
        case EQ: {
            if (!isSingle && first == second) {
                token->type = TK_EQ;
                view = xcs_substring(s, 0, 2);
            } else {
                token->type = TK_ASSIGN;
            }
        }; break;
        case DIV: {
            if (!isSingle && first == second) {
                token->type = TK_IDIV;
                view = xcs_substring(s, 0, 2);
            } else {
                token->type = TK_DIV;
            }
        }; break;
        case SQUIGGLE: {
            if (!isSingle && second == EQ) {
                token->type = TK_NEQ;
                view = xcs_substring(s, 0, 2);
            } else {
                token->type = TK_SQUIGGLE;
            }
        }; break;
    }
XCLL_LLEX_OP_RET:
    token->view = view;
    ctx->didWrite = true;
    ctx->mode = XCLL_MODE_NONE;
    xcs_chop_left(s, view.count);
}


void xcll_lex_keyword(arena_t* mem, XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    // Guaranteed to be a keyword at this point from `xcll_lex`
    XcStringView kwView = xcs_collect(s, xcf_is_alnum_or_underscore);
    char* tmp = ALLOC_ARRAY(mem, char, kwView.count + 1);
    xc_memcpy(tmp, kwView.data, kwView.count);
    int kwHash = hash(tmp, kwView.count);
    token->type = wordToTypeMap[kwHash];
    token->view = kwView;
    ctx->didWrite = true;
    ctx->mode = XCLL_MODE_NONE;
    xcs_chop_left(s, kwView.count);
    arena_pop(mem, kwView.count);
}


void xcll_lex_ident(XcStringView* s, XcLuaToken* token, XcLuaLexContext* ctx) {
    XcStringView identView = xcs_collect(s, xcf_is_alnum_or_underscore);
    token->view = identView;
    token->type = TK_IDENT;
    ctx->didWrite = true;
    ctx->mode = XCLL_MODE_NONE;
    xcs_chop_left(s, identView.count);
}

int get_max_token_length(void) {
    int max_len = 0;
    for (int i = 0; i <= TK_EOF; i++) {
        if (tkTypeStrMap[i]) {
            int len = strlen(tkTypeStrMap[i]);
            if (len > max_len) {
                max_len = len;
            }
        }
    }
    return max_len;
}

void xcll_print_token(XcLuaToken token) {
    static int padding = 0;
    if (padding == 0) {
        padding = get_max_token_length();
    }

    printf("[%llu:%llu]\tType: %-*s | Lexeme: '"XCS_FMT"'\n", 
           token.line, token.col, padding, tkTypeStrMap[token.type], XCS_Arg(token.view));
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
void xcll_lex(arena_t* mem, XcStringView* s, XcLuaToken* token, 
            XcLuaLexContext* ctx, size_t lnno, size_t colno) {
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
     * 4 - Operator Mode
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
    // BUG: as noted in xcll_lex_num, many of these lex functions need to validate the input form and surrounding characters
    // BUG: (ct'd) in general we have been testing for lexing valid input. But we haven't really considered many invalid inputs (i.e. 55555hello => TK_INT_10, TK_IDENT when it should error instead)
    XcLuaToken lexeme = {0};
    *s = xcs_trim_left(s); // Remove leading whitespace
    if (ctx->mode == XCLL_MODE_NONE) {
        // We have just started lexing a new token.
        // Need to determine the mode, set it, then pass control back to the caller.

        /**STRICT:
         * The mode selector should not modify input. Even if it seems unimportant, do not alter
         * input past what `s` is at this point. That is the job of xcll_lex_* functions.
         */
        token->line = lnno;
        token->col = colno;
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

        // operators, keywords, IDENT
        else {
            switch (first) {
                case LT:
                case GT:
                case BWAND:
                case BWOR:
                case EQ:
                case PLUS:
                case MINUS:
                case MUL:
                case DIV:
                case MOD:
                case EXP:
                case LEN:
                case DOT:
                case COMMA:
                case LPAREN:
                case RPAREN:
                case LBRACE:
                case RBRACE:
                case LBRACK:
                case RBRACK:
                case COLON:
                case SEMICOLON:
                case SQUIGGLE: {
                    ctx->mode = XCLL_MODE_OPERATOR;
                }; break;

                default: {
                    // Note that due to xcll_lex_num being handled earlier `s` cannot start
                    // with a number at this point. `s` also can't be empty here either
                    XcStringView kwOrIdent = xcs_collect(s, xcf_is_alnum_or_underscore);
                    if (xcs_empty(&kwOrIdent)) {
                        XCS_LEXER_ERROR("Unrecognized character: '%c'.", token, xcs_at(s, 0));
                    }
                    char* tmp = ALLOC_ARRAY(mem, char, kwOrIdent.count + 1);
                    xc_memcpy(tmp, kwOrIdent.data, kwOrIdent.count);
                    bool isKw = in_word_set(tmp, kwOrIdent.count);
                    arena_pop(mem, kwOrIdent.count);
                    if (isKw) {
                        ctx->mode = XCLL_MODE_KEYWORD;
                    } else {
                        ctx->mode = XCLL_MODE_IDENT;
                    }
                }; break;
            }
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

    if (ctx->mode == XCLL_MODE_OPERATOR) {
        xcll_lex_operator(s, token, ctx);
        goto J_LLEX_WAITING;
    }

    if (ctx->mode == XCLL_MODE_KEYWORD) {
        xcll_lex_keyword(mem, s, token, ctx);
        goto J_LLEX_WAITING;
    }

    if (ctx->mode == XCLL_MODE_IDENT) {
        xcll_lex_ident(s, token, ctx);
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

void setupLineSrcMap(arena_t* mem, const XcStringView* source) {
    size_t nLines = xcs_count(source, '\n') + 1;
    size_t lastLineIdx = 0;
    lineSrcMap = ALLOC_ARRAY(mem, XcStringView, nLines);
    for (size_t i = 0; i < nLines; i++) {
        size_t nextLineIdx = xcs_index_cstr(source, "\n", i);
        if (nextLineIdx == XCS_NOT_FOUND) {
            // EOF
            nextLineIdx = source->count - 1;
        }
        XcStringView line = xcs_substring(
            source,
            lastLineIdx,
            nextLineIdx
        );
        lineSrcMap[i] = line;
        lastLineIdx = nextLineIdx + 1;
    }
}

XcLuaTokens xc_lualex_tokenize(arena_t* mem, FILE* f, const char* fileName) {
    long flen = xc_fs_filelen(f);
    char* buf = ALLOC_ARRAY(mem, char, flen + 1);
    fread(buf, 1, flen, f);
    buf[flen] = 0;
    XcStringView s = xcs(buf);
    Xcltl* list = Xcltl_init();
    size_t lnno = 1, colno = 0;
    XcLuaToken token = {0};
    token.fileName = fileName;
    setupLineSrcMap(mem, &s);

    while (!xcs_empty(&s)) {
        XcStringView old = s;
        xcll_lex(mem, &s, &token, &xcllctx, lnno, colno);
        if (xcllctx.didWrite) {
            Xcltl_append(list, token);
            xcs_chop_right(&old, s.count);
            XcStringView _leadingWhitespace = xcs_collect_until(&s, xcf_not_whitespace);
            old.count += _leadingWhitespace.count;
            size_t linesParsed = xcs_count(&old, '\n');
            int lastLineIdx = xcs_index_cstr(&old, "\n", linesParsed == 0 ? 0 : linesParsed - 1);
            // BUG: Line numbers + column numbers are way off
            lnno += linesParsed;
            colno = lastLineIdx == XCS_NOT_FOUND ? (colno + old.count) : (old.count - lastLineIdx - 1);
            // xcll_print_token(token);
        }
        printf("%p %llu\n", s.data, s.count);
    }
    xcll_lex(mem, &s, &token, &xcllctx, lnno, colno);  // EOF
    Xcltl_append(list, token);

    for (size_t i = 0; i < Xcltl_len(list); i++) {
        XcLuaToken printToken = Xcltl_get(list, i);
        xcll_print_token(printToken); // NOTE: lnno won't work here, need to store per-token lnnos
    }

    return (XcLuaTokens) {
        .n_tokens = 0,
        .tokens = NULL,
    };
}

#endif

#endif