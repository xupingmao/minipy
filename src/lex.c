
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

void lexReadChar(LexState*);

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
LexState* lexNew(FILE* fp, char* buf)
{
	LexState* s = malloc(sizeof(LexState));
	s->cur = 0;
	memset(s->error, 0, sizeof(s->error));
	s->len = 0;
	s->state = 0;
	s->next = 1;
	s->tokenCount = 0;
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
        lexError(s, "no input is defined");
    }
    lexReadChar(s);
	return s;
}

void lexFree(LexState *l){
    free(l->buf);
	free(l);
}

void lexError(LexState *s, char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	vsprintf(s->error, fmt, ap);
	va_end(fmt);
    lexFree(s);
    exit(-1);
}

void lexReadChar(LexState* l){
	if (l->next != EOF){
        l->next = *l->bufp++;
	}else{
		l->type = EOF;
	}
}


void lexReadName(LexState* s)
{
	int p = 0;
	while(isalnum(s->next)){
		s->name[p] = s->next;
		lexReadChar(s);
		p++;
	}
	s->name[p] = '\0';
	s->type = LEX_NAME;
}
/*
 * 0x123
 *  ^
 */
void lexReadHexNum(LexState* s){
	int state = 1;
	lexReadChar(s);
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
		lexReadChar(s);
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
void lexReadNum(LexState* s){
	int p = 0;
	char first = s->next;
	lexReadChar(s);
	/* 16 raidx */
	if(first=='0'&&('x'==s->next||'X'==s->next)){
		lexReadHexNum(s);
		return;
	}else{
		p = 2;
		s->name[0] = first;
		s->name[1] = s->next;
		lexReadChar(s);
	}
	while(isnumber(s->next)){
		s->name[p] = s->next;
		p++;
		lexReadChar(s);
	}
	if(s->next == '.'){
		s->name[p] = '.';
		p++;
		lexReadChar(s);
		while(isnumber(s->next)){
			s->name[p] = s->next;
			p++;
			lexReadChar(s);
		}
	}

	s->name[p] = '\0';
	s->number = atof(s->name);
	s->type = LEX_NUMBER;
}


void lexReadStr(LexState* s, char end) {
	lexReadChar(s);
	int p = 0;
	s->strlen = 0;
	while (s->next != EOF && s->next != end){
		if (s->next == '\\'){
			lexReadChar(s);
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
		lexReadChar(s);
	}
	s->name[p] = '\0';
	s->type = LEX_STRING;

	if (s->next == end){
		lexReadChar(s);
	}
}


void lexSkipWhiteChars(LexState* s){
	if(iswhite(s->next)){
		while(iswhite(s->next)){
			if(s->next=='\n'){
				s->lineno++;
				/* exit if newline is needed */
				if(s->recordrf){
					return;
				}
			}
			lexReadChar(s);
		}
	}
}

void lexSkipChars(LexState* l, int cnt) {
    while(cnt--) {
        lexReadChar(l);
    }
}

void lexReadSymbol(LexState* l) {
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
            lexSkipChars(l, len);
            l->type = s->type;
            strcpy(l->name, s->name);
            return;
        }
    }
    l->type = l->next;
    lexReadChar(l);
}

void lexNext(LexState* s) {
	lexSkipWhiteChars(s);
    if (EOF == s->next) {
        return;
    }
    s->tokenCount ++;
    // LOG("next %c\n", s->next);
    if (isname(s->next)) {
        lexReadName(s);
    } else if (isnumber(s->next)) {
        lexReadNum(s);
    } else if (isstring(s->next)) {
        lexReadStr(s, s->next);
    } else if (issymbol(s->next)) {
        lexReadSymbol(s);
    } else {
        lexError(s, "unknown char type , value = %d\n", s->next);
    }
    // lexSkipWhiteChars(s);
}


void lexPrint(LexState * s)
{
	if (EOF==s->type) {
		return;
	}
	char* sType = "symbol";
	char staticValue[100];
	char* sValue = staticValue;
	switch(s->type){
    case 0:
        sType = "<init>";
        sValue = "init";
        break;
	case LEX_STRING:
		sType = "string";
		strcpy(sValue, s->name);
		break;
	case LEX_NAME:
		sType = "name";
		strcpy(sValue, s->name);
		break;
	case LEX_NUMBER:
		sType="number";
		sprintf(sValue, "%g", s->number);
		break;
	case ',': case ':': case ';':
	case '+': case '-': case '*': case '/': case '%':
	case '>': case '<': case '=':
	case '(': case ')': case '[': 
    case ']': case '{': case '}':
    case '&': case '|':
		charToString(sValue, s->type);
		break;
	case LEX_EQ:sValue="==";break;
	case LEX_GE:sValue=">=";break;
	case LEX_LE:sValue="<=";break;
    case LEX_AND:sValue="AND";break;
    case LEX_OR:sValue="OR";break;
	case '\n': {
        sType="line";
        sprintf(sValue, "[%3d]", s->lineno);
        break;
    }
	default:
		sType="unknown";
	}
	printf("%3d %-10s => %s\n", s->tokenCount, sType, sValue);
}

int main(int argc, char* argv[]){
    FILE* fp;
    // printf("EOF=%d\n", EOF);
    LexState* l = NULL;
    
    if (argc == 1) {
        fp = stdin;
        char buf[1024];
        fgets(buf, 1024, stdin);
        l = lexNew(NULL, buf);
    } else if (argc == 2){
        fp = fopen(argv[1], "rb");
        if (fp == NULL) {
            printf("can not open %s\n", argv[1]);
            return 0;
        }
        l = lexNew(fp, NULL);
    } else {
        printf("usage: %s source\n", argv[0]);
        return 0;
    }
    
	while (1){
        lexNext(l);
        if (l->next == EOF) {
            break;
        } 
        lexPrint(l);
		//Sleep(500);
	}
	printf("lineno=%d\n",l->lineno);
	lexFree(l);
	return 0;
}

