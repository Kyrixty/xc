#ifndef XC_LLEX_H
#define XC_LLEX_H

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
    TK_STR_DBL,
    TK_STR_SGL,
    TK_STR_MUL,
    TK_SYMBOL,      // Used for variable names (i.e. `local myVar = "hello"` => `myVar` is TK_SYMBOL)
    
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

} XcLuaLexer;


#endif