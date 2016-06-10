
#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* set greater than 256 to avoid conflict with ASCII */
#define LEX_NAME 300
#define LEX_NUMBER 301
#define LEX_STRING 302

#define LEX_EQ 400
#define LEX_GE 401
#define LEX_LE 402
#define LEX_AND 403
#define LEX_OR 404

typedef struct {
    int type;
    char s_value[200];
    double d_value;
}Lex_token;

typedef struct Lex_state{
	int cur;
	int next;
	int len;
	int state;
	/* length of string*/
	int strlen;
	const char *fname;
	/* FILE pointer */
	FILE* fp;
    char* buf;
    char* bufp;

	int recordrf; // whether record RF
	int lineno; // line number.
	int token_count; // count of words
	char error[2048];
	int type;
	char name[1024];
	double number;
}Lex_state;

Lex_state* lex_new(FILE* fp, char* buf);
/* Functions */
void lex_next(Lex_state* s);
void lex_error(Lex_state* s, char* fmt, ...);



int iswhite(int c){
    return (c=='\n'||c=='\r'||c==' '||c=='\t');
}


int isname(int c){
    return (c>='a'&&c<='z'||c>='A'&&c<='Z'||c=='_');
}


int isnumber(int c){
    return c>='0'&&c<='9';
}


int isalnum(int c) {
    return isname(c) || isnumber(c);
}


int isstring(int c){
    return c == '\'' || c == '"';
}


int issymbol(int c) {
    static char* string = "~!@#$%%^&()_+-*/[]{}|.<>?;:,=";
    char* s = string;
    while (*s != '\0') {
        if (*s++ == c) return 1;
    }
    return 0;
}


void char_to_string(char* dest, char c){
    dest[0] = c;dest[1] = 0;
}
#endif

