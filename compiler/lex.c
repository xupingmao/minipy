/** 
 * lex.c
 */


#include "lex.h"

typedef struct LexState{
    char *text;
    int length;
    int lookahead;
    int pos;
    int lineno;
    int col;
    int token_type;
    char* token_type_str;
    char token_value[256];
    int token_length;
    int indent_list[10];
    int indent_length;
    int indent;
} LexState;

void lex_clear_token(LexState* lex);
int lex_chk_indent(LexState* lex);
int lex_indent(LexState* lex);

int iswhite(int c){
    return (c=='\n' || c=='\r' || c== ' ' || c=='\t');
}


int isname(int c){
    return (c>='a' && c<='z') || (c >='A' && c <='Z')|| (c =='_');
}


int isquote(int c){
    return c == '\'' || c == '"';
}


int issymbol(int c) {
    static char* string = "~!@#$%%^&()_*+-/[]{}|.<>?;:,=";
    char* s = string;
    while (*s != '\0') {
        if (*s++ == c) return 1;
    }
    return 0;
}

int lex_utf8_len(char* utf8) {
    int count;
        for(count = 0; *utf8; count++) {
        int x = *utf8;
        utf8 += (x & 0x80) ? ((x & 0x20) ? 3 : 2) : 1;
    }
    return count;
}

int lex_utf8_size(unsigned short c) {
  if ((0x0001 <= c) && (c <= 0x007F)) return 1;
  if (c <= 0x07FF) return 2;
  return 3;
}

/*
 * static int names[256];
 * static int numbers[256];
 * names['a..zA..Z_0..9'] = 1;
 * numbers['0..9a..f'] = 1;
 */
LexState* lex_new(char* buf) {
    LexState* s = malloc(sizeof(LexState));
    s->length = strlen(buf);
    s->text = buf;
    s->lookahead = 0;
    s->pos = 0;
    s->lineno = 1;
    s->col = 0;
    s->token_type = 0;
    s->token_type_str = "";
    s->indent_length = 0;
    s->indent = 0;
    memset(s->token_value, 0, sizeof(s->token_value));
    return s;
}

LexState* lex_load(LexState* l, char* str) {
    l->text = str;
    return l;
}

void lex_free(LexState *l){
    free(l);
}

int lex_error(LexState *s, char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    lex_free(s);
    exit(-1);
    return LEX_ERROR;
}

int lex_getc(LexState* l){
    if (l->pos >= l->length) {
        l->lookahead = 0;
        return 0;
    }
    l->lookahead = l->text[l->pos];
    l->pos += 1;
    // skip \r
    if (l->lookahead == '\r') {
        return lex_getc(l);
    }
    return l->lookahead;
}

void lex_clear_token(LexState* l) {
    l->token_length = 0;
    l->token_value[0] = 0;
}

void lex_token_char(LexState* l, int c) {
    l->token_value[l->token_length] = c;
    l->token_length += 1;
    l->token_value[l->token_length] = 0;
}

void lex_token(LexState* l, int type, char* type_str) {
    l->token_type = type;
    l->token_type_str = type_str;
}

/**
 * start lexical analysis
 */
void lex_start(LexState* l) {
    lex_getc(l);
    l->indent_length = 1;
    l->indent_list[0] = 0;
    lex_indent(l);
}

void lex_read_name(LexState* s) {
    lex_clear_token(s);
    while(isalnum(s->lookahead)){
        lex_token_char(s, s->lookahead);
        lex_getc(s);
    }

    s->token_type_str = "name";
    s->token_type = LEX_NAME;
}
/*
 * 0x123
 *  ^
 */
void lex_read_hex_num(LexState* s){
    /*
    int state = 1;
    int c = lex_getc(s);
    long value=0;
    int isNum=1;
    while(isNum){
        if(s->next>='0'&&s->next<='9'){
            value = (value<<4)+s->next-'0';
        }else if(s->next>='a'&&s->next<='f'){
            value = (value<<4)+s->next-'a'+10;
        }else if(s->next>='A'&&s->next<='F'){
            value = (value<<4)+s->next-'A'+10;
        }else{
            isNum=0;
        }
        lex_getc(s);
    }
    s->type = LEX_NUMBER;
    s->number = value;
    */
}

/*
 * eg.
 * 12
 * 12.23
 * 0x123
 */
void lex_read_num(LexState* s){

    lex_clear_token(s);

    /* 16 raidx */

    /*
    if(first=='0' && ('x'==s->lookahead||'X'==s->lookahead)){
        lex_read_hex_num(s);
        return;
    }else{
        p = 2;
        s->name[0] = first;
        s->name[1] = s->next;
        lex_getc(s);
    } */

    while(isnumber(s->lookahead)){
        lex_token_char(s, s->lookahead);
        lex_getc(s);
    }

    if(s->lookahead == '.'){
        lex_token_char(s, '.');
        lex_getc(s);
        while(isnumber(s->lookahead)){
            lex_token_char(s, s->lookahead);
            lex_getc(s);
        }
    }
    s->token_type_str = "number";
    s->token_type = LEX_NUMBER;
}


void lex_read_str(LexState* s) {
    int end = s->lookahead;
    lex_clear_token(s);
    lex_getc(s);
    while (s->lookahead != end) {
        if (s->lookahead == '\\') {
            lex_getc(s);
            switch(s->lookahead) {
                case '0': lex_token_char(s, '\0');break;
                case 'b': lex_token_char(s, '\b');break;
                case 'n': lex_token_char(s, '\n');break;
                case 'r': lex_token_char(s, '\r');break;
                case 't': lex_token_char(s, '\t');break;
                case '"': lex_token_char(s, '\"');break;
                case '\'': lex_token_char(s, '\'');break;
                case '\\': lex_token_char(s, '\\');break;
                default: 
                    lex_token_char(s, '\\');
                    lex_token_char(s, s->lookahead);
                    break;
            }
            lex_getc(s);
        } else {
            lex_token_char(s, s->lookahead);
            lex_getc(s);
        }
    }

    if (s->lookahead == end) {
        lex_getc(s);
    }
    s->token_type = LEX_STRING;
    s->token_type_str = "string";
}

void lex_skip(LexState* l) {
    while (l->lookahead == ' '
        || l->lookahead == '\t') {
        lex_getc(l);
    }
}

/** 
 * indent the text
 * return 1 if indent
 * return -1 if dedent
 * return 0 
 */
int lex_indent(LexState* l) {

    int indent = 0;

    do {
        if (l->lookahead == '\n') {
            lex_getc(l);
            indent = 0;
        }
        while (l->lookahead == ' '
            || l->lookahead == '\t') {
            lex_getc(l);
            indent ++;
        }
    } while (l->lookahead == '\n');

    if (l->indent_length == 0) {
        if (indent > 0) {
            l->indent_list[0] = indent;
            l->indent_length = 1;
            l->indent += 1;  
            return 1;          
        } else {
            return 0;
        }
    } else {
        int last_indent = l->indent_list[l->indent_length-1];
        if (last_indent == indent) {
            // no indent
            return 0;
        } else if (last_indent < indent) {
            // indent
            l->indent_list[l->indent_length] = indent;
            l->indent_length += 1;
            // indent can't be bigger than 1
            l->indent += 1;
            return 1;
        } else {
            // indent_list = [4, 8, 12]
            // indent = 4
            // length = 3
            // i = 1
            // pos = 0
            // return pos - i - 1
            int i = l->indent_length-2;
            int pos = -1;
            l->indent -= 1; // must dedent at least 1.
            while (i >= 0) {
                if (l->indent_list[i] == indent) {
                    pos = i;
                    break;
                } else {
                    l->indent -= 1;
                }
                i--;
            }
            if (pos >= 0) {
                return -1;
            } else {
                return lex_error(l, "indent error");
            }
        }
    }
}

int lex_chk_indent(LexState* s) {
    if (s->indent < 0) {
        lex_clear_token(s);
        lex_token(s, LEX_DEDENT, "dedent");
        s->indent++;
        return 1;
    } else if (s->indent > 0) {
        lex_clear_token(s);
        lex_token(s, LEX_INDENT, "indent");
        s->indent--;
        return 1;
    }
    return 0;
}

void lex_symbol(LexState* l, char* token_str) {
    lex_token_char(l, l->lookahead);
    lex_getc(l);
    lex_token(l, token_str[0], token_str);
}

void lex_symbol_maybe_eq(LexState* l, char* token_str, 
        int eq_token_type,
        char* eq_token_str) {
    lex_token_char(l, l->lookahead);
    lex_getc(l);
    if (l->lookahead == '=') {
        lex_token_char(l, l->lookahead);
        lex_getc(l);
        lex_token(l, eq_token_type, eq_token_str);
    } else {
        lex_token(l, token_str[0], token_str);
    }
}

void lex_read_symbol(LexState* l) {
    lex_clear_token(l);

    switch (l->lookahead) {
        case '(': lex_symbol(l, "("); break;
        case ')': lex_symbol(l, ")"); break;
        case '{': lex_symbol(l, "{"); break;
        case '}': lex_symbol(l, "}"); break;
        case ',': lex_symbol(l, ","); break;
        case ':': lex_symbol(l, ":"); break;
        case '?': lex_symbol(l, "?"); break;
        case '.': lex_symbol(l, "."); break;
        case '+': lex_symbol_maybe_eq(l, "+", LEX_ADD_EQ, "+=");break;
        case '-': lex_symbol_maybe_eq(l, "-", LEX_SUB_EQ, "-=");break;
        case '*': lex_symbol_maybe_eq(l, "*", LEX_MUL_EQ, "*=");break;
        case '/': lex_symbol_maybe_eq(l, "/", LEX_DIV_EQ, "/=");break;
        case '=': lex_symbol_maybe_eq(l, "=", LEX_EQEQ, "==");break;
        case '>': lex_symbol_maybe_eq(l, ">", LEX_GTEQ, ">=");break;
        case '<': lex_symbol_maybe_eq(l, "<", LEX_LTEQ, "<=");break;
        case '!': lex_symbol_maybe_eq(l, "!", LEX_NEQ, "!=");break;
        default:
            lex_token_char(l, l->lookahead);
            lex_getc(l);
            lex_token(l, LEX_UNKNOWN, "unknown");
            break;
    }
}

void lex_next(LexState* s) {

    if (lex_chk_indent(s)) {
        return;
    }

    lex_skip(s);
    if (0 == s->lookahead) {
        lex_token(s, LEX_EOF, "eof");
        return;
    }

    if (s->lookahead == '\n') {
        lex_getc(s);
        lex_indent(s);
        if (lex_chk_indent(s)) {
            return;
        }
    }

    // LOG("next %c\n", s->next);
    if (isname(s->lookahead)) {
        lex_read_name(s);
    } else if (isnumber(s->lookahead)) {
        lex_read_num(s);
    } else if (isquote(s->lookahead)) {
        lex_read_str(s);
    } else if (issymbol(s->lookahead)) {
        lex_read_symbol(s);
    } else {
        lex_error(s, "unknown char type , value = %d\n", s->lookahead);
    }
    // lex_skip_whites(s);
}

void lex_test() {
    Object obj = tm_load("lex.txt");
    char* sz = GET_SZ(obj);
    LexState* l = lex_new(sz);
    lex_start(l);

    while (1){
        lex_next(l);
        printf("%-10s : %s\n", l->token_type_str, l->token_value);
        if (l->lookahead == 0) {
            break;
        } 
        //Sleep(500);
    }
    lex_free(l);
}

int main(int argc, char* argv[]){
    tm_run_func(argc, argv, "lex_test", lex_test);
    return 0;
}

