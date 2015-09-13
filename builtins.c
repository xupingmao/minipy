#include "include/builtins.h"
#include "include/tmstring.h"
#include "include/gc.h"
#include "include/object.h"
#include "include/StringBuilder.h"
#include <ctype.h>
#ifdef TM_NT
	#include <windows.h>
#else
	#include <unistd.h>
#endif

Object* getBuiltin(char* key) {
    if (!tm->init) {
        return NULL;
    }
	return dictGetByStr(tm->builtins, key);
}

void tmPutchar(int c){
	static char hex[] = {
		'0','1','2','3','4','5','6','7',
		'8','9','A','B','C','D','E','F'
	};
	if (isprint(c)) {
		putchar(c);
	} else if(c== '\n' || c == '\t'){
		putchar(c);
	} else if(c == '\b') {
		putchar('\b');
	} else if(c=='\r'){
		/* output nothing */
	} else {
		int c0 = (c & 0xF0) >> 4;
		int c1 = c & 0x0F;
		printf("0x%c%c", hex[c0], hex[c1]);
	}
}

Object tmStr(Object a) {
	char buf[100];
	memset(buf, 0, sizeof(buf));
	switch (TM_TYPE(a)) {
	case TYPE_STR:
		return a;
	case TYPE_NUM: {
		char s[20];
		double v = GET_NUM(a);
		numberFormat(s, a);
		return string_new(s);
	}
	case TYPE_LIST: {
		StringBuilder* sb = StringBuilderNew();
		StringBuilderAppend(sb, '[');
		int i, l = LIST_LEN(a);
		for (i = 0; i < l; i++) {
			Object obj = GET_LIST(a)->nodes[i];
			/* reference to self in list */
			if (tmEquals(a, obj)) {
				StringBuilderAppendChars(sb, "[...]");
			} else if (obj.type == TYPE_STR) {
				StringBuilderAppend(sb, '"');
				StringBuilderAppendObj(sb, obj);
				StringBuilderAppend(sb, '"');
			} else {
				StringBuilderAppendObj(sb, obj);
			}
			if (i != l - 1)
				StringBuilderAppend(sb, ',');
		}
		StringBuilderAppend(sb, ']');
		return StringBuilderToStr(sb);
	}
	case TYPE_DICT:
		sprintf(buf, "<dict at %p>", GET_DICT(a));
		return string_new(buf);
	case TYPE_FUNCTION:
		functionFormat(buf, a);
		return string_new(buf);
	case TYPE_NONE:
		return string_static("None");
	case TYPE_DATA:
		return GET_DATA_PROTO(a)->str(GET_DATA(a));
	default:
		tmRaise("str: not supported type %d", a.type);
	}
	return string_alloc("", 0);
}

void tmPrint(Object o) {
	Object str = tmStr(o);
	int i;
	for(i = 0; i < GET_STR_LEN(str); i++) {
		tmPutchar(GET_STR(str)[i]);
	}
}

void tmPrintln(Object o) {
	tmPrint(o);
	puts("");
}



/**
 * based on C language standard.
 * t，Object
 * i，Object infomation, mainly for String
 * l, information of list.
 */
Object tmFormatVaList(char* fmt, va_list ap, int acquireNewLine) {
	int i;
	int len = strlen(fmt);
	Object nstr = string_static("");
	int templ = 0;
	char* start = fmt;
	int istrans = 1;
	StringBuilder* sb = StringBuilderNew();
	for (i = 0; i < len; i++) {
		if (fmt[i] == '%') {
			i++;
			switch (fmt[i]) {
			case 'd':
				StringBuilderAppendInt(sb, va_arg(ap, int));
				break;
			case 'f':
				/* ... will pass float as double */
				StringBuilderAppendDouble(sb, va_arg(ap, double));
				break;
				/* ... will pass char  as int */
			case 'c':
				StringBuilderAppend(sb, va_arg(ap, int));
				break;
			case 's': {
				StringBuilderAppendChars(sb, va_arg(ap, char*));
				break;
			}
			case 'P':
			case 'p': {
				StringBuilderAppendPointer(sb, va_arg(ap, void*));
				break;
			}
			case 'o': {
				Object v = va_arg(ap, Object);
				if (IS_STR(v)) {
					StringBuilderAppend(sb, '"');
				}
				StringBuilderAppendObj(sb, v);
				if (IS_STR(v)) {
					StringBuilderAppend(sb, '"');
				}
				break;
			}
			default:
				tmRaise("format, unknown pattern %c", fmt[i]);
				break;
			}
		} else {
			StringBuilderAppend(sb, fmt[i]);
		}
	}
	if (acquireNewLine)
		StringBuilderAppend(sb, '\n');
	return StringBuilderToStr(sb);
}

Object tmFormat(char* fmt, ...) {
	va_list a;
	va_start(a, fmt);
	Object v = tmFormatVaList(fmt, a, 0);
	va_end(a);
	return v;
}

void tmPrintf(char* fmt, ...) {
	va_list a;
	va_start(a, fmt);
	tmPrint(tmFormatVaList(fmt, a, 0));
	va_end(a);
}


long getRestSize(FILE* fp){
	long cur, end;
	cur = ftell(fp);
	fseek(fp, 0, SEEK_END);
	end = ftell(fp);
	fseek(fp, cur, SEEK_SET);
	return end - cur;
}


Object tmLoad(char* fname){
	FILE* fp = fopen(fname, "rb");
	if(fp == NULL){
		tmRaise("load: can not open file \"%s\"",fname);
		return NONE_OBJECT;
	}
	long len = getRestSize(fp);
	if(len > MAX_FILE_SIZE){
		tmRaise("load: file too big to load, size = %d", (len));
		return NONE_OBJECT;
	}
	Object text = string_alloc(NULL, len);
	char* s = GET_STR(text);
	fread(s, 1, len, fp);
	fclose(fp);
	return text;
}

Object tmSave(char*fname, Object content) {
	FILE* fp = fopen(fname, "wb");
	if (fp == NULL) {
		tmRaise("tmSave : can not save to file \"%s\"", fname);
	}
	char* txt = GET_STR(content);
	int len = GET_STR_LEN(content);
	fwrite(txt, 1, len, fp);
	fclose(fp);
	return NONE_OBJECT;
}


Object bfInput() {
	int i = 0;
	if (hasArg()) {
		tmPrint(getObjArg("input"));
	}
	char buf[2048];
	memset(buf, '\0', sizeof(buf));
	fgets(buf, sizeof(buf), stdin);
	int len = strlen(buf);
	/* if last char is '\n', we shift it, mainly in tcc */
	if(buf[len-1]=='\n'){
		buf[len-1] = '\0';
	}
	return string_new(buf);
}

Object bfInt() {
	Object v = getObjArg("int");
	if (v.type == TYPE_NUM) {
		return newNumber((int) GET_NUM(v));
	} else if (v.type == TYPE_STR) {
		return newNumber((int) atof(GET_STR(v)));
	}
	tmRaise("int: %o can not be parsed to int ", v);
	return NONE_OBJECT;
}

Object bfFloat() {
	Object v = getObjArg("float");
	if (v.type == TYPE_NUM) {
		return v;
	} else if (v.type == TYPE_STR) {
		return newNumber(atof(GET_STR(v)));
	}
	tmRaise("float: %o can not be parsed to float", v);
	return NONE_OBJECT;
}

/**
 *   load_module( name, code, mod_name = None )
 */
Object bfLoadModule() {
	const char* szFnc = "load_module";
	Object file = getStrArg(szFnc);
	Object code = getStrArg(szFnc);
	Object mod;
	if (getArgsCount() == 3) {
		mod = moduleNew(file, getStrArg(szFnc), code);
	} else {
		mod = moduleNew(file, file, code);
	}
	Object fnc = newFunction(mod, NONE_OBJECT, NULL);
	GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
	GET_FUNCTION(fnc)->name = string_new("#main");
	callFunction(fnc);
	return GET_MODULE(mod)->globals;
}


/* get globals */
Object bfGlobals() {
	return GET_FUNCTION_GLOBALS(tm->frame->fnc);
}

/* get object type */

Object bfExit() {
	longjmp(tm->frames->buf, 2);
	return NONE_OBJECT;
}

Object bfGetType() {
	Object obj = getObjArg("gettype");
	switch(TM_TYPE(obj)) {
		case TYPE_STR: return string_static("string");
		case TYPE_NUM: return string_static("number");
		case TYPE_LIST: return string_static("list");
		case TYPE_DICT: return string_static("dict");
		case TYPE_FUNCTION: return string_static("function");
		case TYPE_DATA: return string_static("data");
		case TYPE_NONE: return string_static("None");
		default: tmRaise("gettype(%o)", obj);
    }
	return NONE_OBJECT;
}

Object bfChr() {
	int n = getIntArg("chr");
	return string_chr(n);
}

Object bfOrd() {
	Object c = getStrArg("ord");
	TM_ASSERT(GET_STR_LEN(c) == 1, "ord() expected a character");
	return newNumber((unsigned char) GET_STR(c)[0]);
}

Object bfCode8() {
	int n = getIntArg("code8");
	if (n < 0 || n > 255)
		tmRaise("code8(): expect number 0-255, but see %d", n);
	return string_chr(n);
}

Object bfCode16() {
	int n = getIntArg("code16");
	if (n < 0 || n > 0xffff)
		tmRaise("code16(): expect number 0-0xffff, but see %x", n);
	Object nchar = string_alloc(NULL, 2);
	code16((unsigned char*) GET_STR(nchar), n);
	return nchar;
}

Object bfCode32() {
	int n = getIntArg("code32");
	Object c = string_alloc(NULL, 4);
	code32((unsigned char*) GET_STR(c), n);
	return c;
}

Object bfRaise() {
	if (getArgsCount() == 0) {
		tmRaise("raise");
	} else {
		tmRaise("%s", getSzArg("raise"));
	}
	return NONE_OBJECT;
}

Object bfSystem() {
	Object m = getStrArg("system");
	int rs = system(GET_STR(m));
	return newNumber(rs);
}

Object bfStr() {
	Object a = getObjArg("str");
	return tmStr(a);
}

Object bfLen() {
	Object o = getObjArg("len");
    int len = -1;
    switch (TM_TYPE(o)) {
	case TYPE_STR:
		len = GET_STR_LEN(o);
	case TYPE_LIST:
		len = LIST_LEN(o);
	case TYPE_DICT:
		len = DICT_LEN(o);
	}
    if (len < 0) {
        tmRaise("tmLen: %o has no attribute len", o);
    }
	return newNumber(len);
}

Object bfPrint() {
	int i = 0;
	while (hasArg()) {
		tmPrint(getObjArg("print"));
		if (hasArg()) {
			putchar(' ');
		}
	}
	putchar('\n');
	return NONE_OBJECT;
}

Object bfLoad(Object p){
	Object fname = getStrArg("load");
	return tmLoad(GET_STR(fname));
}
Object bfSave(){
	Object fname = getStrArg("<save name>");
	return tmSave(GET_STR(fname), getStrArg("<save content>"));
}

Object bfRemove(){
    Object fname = getStrArg("remove");
    int flag = remove(GET_STR(fname));
    if(flag) {
    	return newNumber(0);
    } else {
    	return newNumber(1);
    }
}

Object bfApply() {
	Object func = getObjArg("apply");
    if (NOT_FUNC(func) && NOT_DICT(func)) {
        tmRaise("apply: expect function or dict");
    }
	Object args = getObjArg("apply");
	argStart();
	if (IS_NONE(args)) {
	} else if(IS_LIST(args)) {
		int i;for(i = 0; i < LIST_LEN(args); i++) {
			pushArg(LIST_NODES(args)[i]);
		}
	} else {
		tmRaise("apply: expect list arguments or None, but see %o", args);
        return NONE_OBJECT;
	}
    return callFunction(func);
}

Object bfWrite() {
	Object fmt = getObjArg("puts");
	Object str = tmStr(fmt);
    char* s = GET_STR(str);
    int len = GET_STR_LEN(str);
    int i;
    for(i = 0; i < len; i++) {
        tmPutchar(s[i]);
    }
    return NONE_OBJECT;
}

Object bfPow() {
    double base = getNumArg("pow");
    double y = getNumArg("pow");
    return newNumber(pow(base, y));
}

typedef struct RangeIter {
	DATA_HEAD
	long cur;
    long inc;
    long stop;
    Object cur_obj;
}RangeIter;

Object* rangeNext(RangeIter* data) {
    long cur = data->cur;
    if (data->inc > 0 && cur < data->stop) {
        data->cur += data->inc;
        data->cur_obj = newNumber(cur);
        return &data->cur_obj;
    } else if (data->inc < 0 && cur > data->stop) {
        data->cur += data->inc;
        data->cur_obj = newNumber(cur);
        return &data->cur_obj;
    }
    return NULL;
}

static DataProto rangeIter;
DataProto* getRangeIterProto() {
	if(!rangeIter.init) {
		initDataProto(&rangeIter);
		rangeIter.next = rangeNext;
		rangeIter.dataSize = sizeof(RangeIter);
	}
	return &rangeIter;
}

Object bfRange() {
	long start = 0;
	long end = 0;
	int inc;
    static const char* szFunc = "range";
	switch (tm->arg_cnt) {
	case 1:
		start = 0;
		end = (long)getNumArg(szFunc);
		inc = 1;
		break;
	case 2:
		start = (long)getNumArg(szFunc);
		end = (long)getNumArg(szFunc);
		inc = 1;
		break;
	case 3:
		start = (long)getNumArg(szFunc);
		end = (long)getNumArg(szFunc);
		inc = (long)getNumArg(szFunc);
		break;
	default:
		tmRaise("range([n, [ n, [n]]]), but see %d arguments",
				tm->arg_cnt);
	}
	if (inc == 0)
		tmRaise("range(): increment can not be 0!");
	if (inc * (end - start) < 0)
		tmRaise("range(%d, %d, %d): not valid range!", start, end, inc);
    Object data = dataNew(sizeof(RangeIter));
    RangeIter *iterator = (RangeIter*) GET_DATA(data);
	iterator->cur = start;
	iterator->stop = end;
    iterator->inc = inc;
	iterator->proto = getRangeIterProto();
    return data;
}


void regBuiltinsFunc() {
	regBuiltinFunc("load", bfLoad);
	regBuiltinFunc("save", bfSave);
    regBuiltinFunc("remove", bfRemove);
	regBuiltinFunc("print", bfPrint);
	regBuiltinFunc("write", bfWrite);
	regBuiltinFunc("input", bfInput);
	regBuiltinFunc("str", bfStr);
	regBuiltinFunc("int", bfInt);
	regBuiltinFunc("float", bfFloat);
	regBuiltinFunc("load_module", bfLoadModule);
	regBuiltinFunc("globals", bfGlobals);
	regBuiltinFunc("len", bfLen);
	regBuiltinFunc("exit", bfExit);
	regBuiltinFunc("gettype", bfGetType);
	regBuiltinFunc("chr", bfChr);
	regBuiltinFunc("ord", bfOrd);
	regBuiltinFunc("code8", bfCode8);
	regBuiltinFunc("code16", bfCode16);
	regBuiltinFunc("code32", bfCode32);
	regBuiltinFunc("raise", bfRaise);
	regBuiltinFunc("system", bfSystem);
	regBuiltinFunc("apply", bfApply);
    regBuiltinFunc("pow", bfPow);
    regBuiltinFunc("range", bfRange);
}

