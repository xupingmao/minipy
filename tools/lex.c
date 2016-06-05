
#include "include/lex.h"
/**
 * @desc lexical parser
 * @Author xupingmao
 * @Date 2015-11-02 19:41:44
 */

void LOG(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    //vprintf(fmt, ap);
    va_end(ap);
}

typedef struct _sym {
    char *name;
    int type;
} Symbol;

static Symbol SYM_TAB[] = {
    {"&&", LEX_AND},
    {"||", LEX_OR}, 
    {">=", LEX_GE},
    {"<=", LEX_LE},
    {"==", LEX_EQ},
    {NULL, 0}
};

void lex_read_char(Lex_state*);

int lex_utf8_len(char* utf8) {
    int count;
        for(count = 0; *utf8; count++) {
        int x = *utf8;
        utf8 += (x & 0x80) ? ((x & 0x20) ? 3 : 2) : 1;
    }
    return count;
}

int lex_utf8_size(unsigned short c) {
  if ((0x0001 <= c) && (c <= 0x007_f)) return 1;
  if (c <= 0x07_f_f) return 2;
  return 3;
}

/*
 * static int names[256];
 * static int numbers[256];
 * names['a..z_a..Z_0..9'] = 1;
 * numbers['0..9a..f'] = 1;
 */
Lex_state* lex_new(FILE* fp, char* buf)
{
	Lex_state* s = malloc(sizeof(Lex_state));
	s->cur = 0;
	memset(s->error, 0, sizeof(s->error));
	s->len = 0;
	s->state = 0;
	s->next = 1;
	s->token_count = 0;
	s->lineno = 1;
	s->recordrf = 0;
    s->fp = fp;
    s->buf = NULL;
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        s->buf = malloc(size + 1);
        fseek(fp, 0, SEEK_SET);
        fread(s->buf, size, 1, fp);
        s->bufp = s->buf;
        s->buf[size] = EOF;
        fclose(fp);
    }
    else if (buf != NULL) {
        int size = strlen(buf);
        s->buf = buf;
        s->bufp = s->buf;
        s->buf[size] = EOF;
    } 
    if (s->buf == NULL) {
        lex_error(s, "no input is defined");
    }
    lex_read_char(s);
	return s;
}

void lex_free(Lex_state *l){
    free(l->buf);
	free(l);
}

void lex_error(Lex_state *s, char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	vsprintf(s->error, fmt, ap);
	va_end(fmt);
    lex_free(s);
    exit(-1);
}

void lex_read_char(Lex_state* l){
	if (l->next != EOF){
        l->next = *l->bufp++;
	}else{
		l->type = EOF;
	}
}


void lex_read_name(Lex_state* s)
{
	int p = 0;
	while(isalnum(s->next)){
		s->name[p] = s->next;
		lex_read_char(s);
		p++;
	}
	s->name[p] = '\0';
	s->type = LEX_NAME;
}
/*
 * 0x123
 *  ^
 */
void lex_read_hex_num(Lex_state* s){
	int state = 1;
	lex_read_char(s);
	long value=0;
	int is_num=1;
	while(is_num){
		if(s->next>='0'&&s->next<='9'){
			value = (value<<4)+s->next-'0';
		}else if(s->next>='a'&&s->next<='f'){
			value = (value<<4)+s->next-'a'+10;
		}else if(s->next>='A'&&s->next<='F'){
			value = (value<<4)+s->next-'A'+10;
		}else{
			is_num=0;
		}
		lex_read_char(s);
	}
	s->type = LEX_NUMBER;
	s->number = value;
}

/*
 * eg.
 * 12
 * 12.23
 * 0x123
 */
void lex_read_num(Lex_state* s){
	int p = 0;
	char first = s->next;
	lex_read_char(s);
	/* 16 raidx */
	if(first=='0'&&('x'==s->next||'X'==s->next)){
		lex_read_hex_num(s);
		return;
	}else{
		p = 2;
		s->name[0] = first;
		s->name[1] = s->next;
		lex_read_char(s);
	}
	while(isnumber(s->next)){
		s->name[p] = s->next;
		p++;
		lex_read_char(s);
	}
	if(s->next == '.'){
		s->name[p] = '.';
		p++;
		lex_read_char(s);
		while(isnumber(s->next)){
			s->name[p] = s->next;
			p++;
			lex_read_char(s);
		}
	}

	s->name[p] = '\0';
	s->number = atof(s->name);
	s->type = LEX_NUMBER;
}


void lex_read_str(Lex_state* s, char end) {
	lex_read_char(s);
	int p = 0;
	s->strlen = 0;
	while (s->next != EOF && s->next != end){
		if (s->next == '\\'){
			lex_read_char(s);
			switch( s->next ){
			case '0':s->next = '\0';break;
			case 'b':s->next = '\b';break;
			case 'n':s->next = '\n';break;
			case 'r':s->next = '\r';break;
			case 't':s->next = '\t';break;
			case '"':s->next = '"';break;
			case '\'':s->next = '\'';break;
			}
		}
		s->name[p] = s->next;
		p++;
		s->strlen ++;
		lex_read_char(s);
	}
	s->name[p] = '\0';
	s->type = LEX_STRING;

	if (s->next == end){
		lex_read_char(s);
	}
}


void lex_skip_white_chars(Lex_state* s){
	if(iswhite(s->next)){
		while(iswhite(s->next)){
			if(s->next=='\n'){
				s->lineno++;
				/* exit if newline is needed */
				if(s->recordrf){
					return;
				}
			}
			lex_read_char(s);
		}
	}
}

void lex_skip_chars(Lex_state* l, int cnt) {
    while(cnt--) {
        lex_read_char(l);
    }
}

void lex_read_symbol(Lex_state* l) {
    Symbol* s;
    char buf[10];
    char* lex_buf = l->bufp;
    buf[0] = l->next;
    strncpy(buf+1, lex_buf, 9);
    LOG("get %c, buf=%s\n", buf[0], buf);
    for (s = SYM_TAB ; s->name != NULL; s++) {
        int len = strlen(s->name);
        if (strncmp(buf, s->name, len) == 0) {
            LOG("get %s\n", s->name);
            lex_skip_chars(l, len);
            l->type = s->type;
            strcpy(l->name, s->name);
            return;
        }
    }
    l->type = l->next;
    lex_read_char(l);
}

void lex_next(Lex_state* s) {
	lex_skip_white_chars(s);
    if (EOF == s->next) {
        return;
    }
    s->token_count ++;
    // LOG("next %c\n", s->next);
    if (isname(s->next)) {
        lex_read_name(s);
    } else if (isnumber(s->next)) {
        lex_read_num(s);
    } else if (isstring(s->next)) {
        lex_read_str(s, s->next);
    } else if (issymbol(s->next)) {
        lex_read_symbol(s);
    } else {
        lex_error(s, "unknown char type , value = %d\n", s->next);
    }
    // lex_skip_white_chars(s);
}


void lex_print(Lex_state * s)
{
	if (EOF==s->type) {
		return;
	}
	char* s_type = "symbol";
	char static_value[100];
	char* s_value = static_value;
	switch(s->type){
    case 0:
        s_type = "<init>";
        s_value = "init";
        break;
	case LEX_STRING:
		s_type = "string";
		strcpy(s_value, s->name);
		break;
	case LEX_NAME:
		s_type = "name";
		strcpy(s_value, s->name);
		break;
	case LEX_NUMBER:
		s_type="number";
		sprintf(s_value, "%g", s->number);
		break;
	case ',': case ':': case ';':
	case '+': case '-': case '*': case '/': case '%':
	case '>': case '<': case '=':
	case '(': case ')': case '[': 
    case ']': case '{': case '}':
    case '&': case '|':
		char_to_string(s_value, s->type);
		break;
	case LEX_EQ:s_value="==";break;
	case LEX_GE:s_value=">=";break;
	case LEX_LE:s_value="<=";break;
    case LEX_AND:s_value="AND";break;
    case LEX_OR:s_value="OR";break;
	case '\n': {
        s_type="line";
        sprintf(s_value, "[%3d]", s->lineno);
        break;
    }
	default:
		s_type="unknown";
	}
	printf("%3d %-10s => %s\n", s->token_count, s_type, s_value);
}

int main(int argc, char* argv[]){
    FILE* fp;
    // printf("EOF=%d\n", EOF);
    Lex_state* l = NULL;
    
    if (argc == 1) {
        fp = stdin;
        char buf[1024];
        fgets(buf, 1024, stdin);
        l = lex_new(NULL, buf);
    } else if (argc == 2){
        fp = fopen(argv[1], "rb");
        if (fp == NULL) {
            printf("can not open %s\n", argv[1]);
            return 0;
        }
        l = lex_new(fp, NULL);
    } else {
        printf("usage: %s source\n", argv[0]);
        return 0;
    }
    
	while (1){
        lex_next(l);
        if (l->next == EOF) {
            break;
        } 
        lex_print(l);
		//Sleep(500);
	}
	printf("lineno=%d\n",l->lineno);
	lex_free(l);
	return 0;
}













