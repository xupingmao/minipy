#include <stdarg.h>
#include <stdio.h> 

enum {

    LEX_NAME,
    LEX_NUMBER,
    LEX_STRING,
    LEX_ASSIGNMENT,
    LEX_LPAREN,
    LEX_RPAREN,
    LEX_LBRACKET,
    LEX_RBRACKET,
    LEX_COMMA,

    LEX_EQEQ,
    LEX_LTEQ,
    LEX_GTEQ,
    LEX_NEQ,

    LEX_ADD_EQ,
    LEX_SUB_EQ,
    LEX_MUL_EQ,
    LEX_DIV_EQ,

    LEX_INDENT,
    LEX_DEDENT,
    LEX_UNKNOWN,
    LEX_EOF,
    LEX_ERROR

} LexType;