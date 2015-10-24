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

void tm_putchar(int c){
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
			if (tm_equals(a, obj)) {
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
		tm_raise("str: not supported type %d", a.type);
	}
	return string_alloc("", 0);
}

void tm_print(Object o) {
	Object str = tmStr(o);
	int i;
	for(i = 0; i < GET_STR_LEN(str); i++) {
		tm_putchar(GET_STR(str)[i]);
	}
}

void tm_println(Object o) {
	tm_print(o);
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
				tm_raise("format, unknown pattern %c", fmt[i]);
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

void tm_printf(char* fmt, ...) {
	va_list a;
	va_start(a, fmt);
	tm_print(tmFormatVaList(fmt, a, 0));
	va_end(a);
}


long get_rest_size(FILE* fp){
	long cur, end;
	cur = ftell(fp);
	fseek(fp, 0, SEEK_END);
	end = ftell(fp);
	fseek(fp, cur, SEEK_SET);
	return end - cur;
}


Object tm_load(char* fname){
	FILE* fp = fopen(fname, "rb");
	if(fp == NULL){
		tm_raise("load: can not open file \"%s\"",fname);
		return NONE_OBJECT;
	}
	long len = get_rest_size(fp);
	if(len > MAX_FILE_SIZE){
		tm_raise("load: file too big to load, size = %d", (len));
		return NONE_OBJECT;
	}
	Object text = string_alloc(NULL, len);
	char* s = GET_STR(text);
	fread(s, 1, len, fp);
	fclose(fp);
	return text;
}

Object tm_save(char*fname, Object content) {
	FILE* fp = fopen(fname, "wb");
	if (fp == NULL) {
		tm_raise("tm_save : can not save to file \"%s\"", fname);
	}
	char* txt = GET_STR(content);
	int len = GET_STR_LEN(content);
	fwrite(txt, 1, len, fp);
	fclose(fp);
	return NONE_OBJECT;
}


Object bf_input() {
	int i = 0;
	if (hasArg()) {
		tm_print(arg_get_obj("input"));
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

Object bf_int() {
	Object v = arg_get_obj("int");
	if (v.type == TYPE_NUM) {
		return tm_number((int) GET_NUM(v));
	} else if (v.type == TYPE_STR) {
		return tm_number((int) atof(GET_STR(v)));
	}
	tm_raise("int: %o can not be parsed to int ", v);
	return NONE_OBJECT;
}

Object bf_float() {
	Object v = arg_get_obj("float");
	if (v.type == TYPE_NUM) {
		return v;
	} else if (v.type == TYPE_STR) {
		return tm_number(atof(GET_STR(v)));
	}
	tm_raise("float: %o can not be parsed to float", v);
	return NONE_OBJECT;
}

/**
 *   load_module( name, code, mod_name = None )
 */
Object bf_load_module() {
	const char* szFnc = "load_module";
	Object file = arg_get_str(szFnc);
	Object code = arg_get_str(szFnc);
	Object mod;
	if (getArgsCount() == 3) {
		mod = module_new(file, arg_get_str(szFnc), code);
	} else {
		mod = module_new(file, file, code);
	}
	Object fnc = func_new(mod, NONE_OBJECT, NULL);
	GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
	GET_FUNCTION(fnc)->name = string_new("#main");
	callFunction(fnc);
	return GET_MODULE(mod)->globals;
}


/* get globals */
Object bf_globals() {
	return GET_FUNCTION_GLOBALS(tm->frame->fnc);
}

/* get object type */

Object bf_exit() {
	longjmp(tm->frames->buf, 2);
	return NONE_OBJECT;
}

Object bf_gettype() {
	Object obj = arg_get_obj("gettype");
	switch(TM_TYPE(obj)) {
		case TYPE_STR: return string_static("string");
		case TYPE_NUM: return string_static("number");
		case TYPE_LIST: return string_static("list");
		case TYPE_DICT: return string_static("dict");
		case TYPE_FUNCTION: return string_static("function");
		case TYPE_DATA: return string_static("data");
		case TYPE_NONE: return string_static("None");
		default: tm_raise("gettype(%o)", obj);
    }
	return NONE_OBJECT;
}

Object bf_chr() {
	int n = arg_get_int("chr");
	return string_chr(n);
}

Object bf_ord() {
	Object c = arg_get_str("ord");
	TM_ASSERT(GET_STR_LEN(c) == 1, "ord() expected a character");
	return tm_number((unsigned char) GET_STR(c)[0]);
}

Object bf_code8() {
	int n = arg_get_int("code8");
	if (n < 0 || n > 255)
		tm_raise("code8(): expect number 0-255, but see %d", n);
	return string_chr(n);
}

Object bf_code16() {
	int n = arg_get_int("code16");
	if (n < 0 || n > 0xffff)
		tm_raise("code16(): expect number 0-0xffff, but see %x", n);
	Object nchar = string_alloc(NULL, 2);
	code16((unsigned char*) GET_STR(nchar), n);
	return nchar;
}

Object bf_code32() {
	int n = arg_get_int("code32");
	Object c = string_alloc(NULL, 4);
	code32((unsigned char*) GET_STR(c), n);
	return c;
}

Object bf_raise() {
	if (getArgsCount() == 0) {
		tm_raise("raise");
	} else {
		tm_raise("%s", arg_get_sz("raise"));
	}
	return NONE_OBJECT;
}

Object bf_system() {
	Object m = arg_get_str("system");
	int rs = system(GET_STR(m));
	return tm_number(rs);
}

Object bf_str() {
	Object a = arg_get_obj("str");
	return tmStr(a);
}

Object bf_len() {
	Object o = arg_get_obj("len");
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
        tm_raise("tmLen: %o has no attribute len", o);
    }
	return tm_number(len);
}

Object bf_print() {
	int i = 0;
	while (hasArg()) {
		tm_print(arg_get_obj("print"));
		if (hasArg()) {
			putchar(' ');
		}
	}
	putchar('\n');
	return NONE_OBJECT;
}

Object bfLoad(Object p){
	Object fname = arg_get_str("load");
	return tm_load(GET_STR(fname));
}
Object bfSave(){
	Object fname = arg_get_str("<save name>");
	return tm_save(GET_STR(fname), arg_get_str("<save content>"));
}

Object bfRemove(){
    Object fname = arg_get_str("remove");
    int flag = remove(GET_STR(fname));
    if(flag) {
    	return tm_number(0);
    } else {
    	return tm_number(1);
    }
}

Object bf_apply() {
	Object func = arg_get_obj("apply");
    if (NOT_FUNC(func) && NOT_DICT(func)) {
        tm_raise("apply: expect function or dict");
    }
	Object args = arg_get_obj("apply");
	arg_start();
	if (IS_NONE(args)) {
	} else if(IS_LIST(args)) {
		int i;for(i = 0; i < LIST_LEN(args); i++) {
			arg_push(LIST_NODES(args)[i]);
		}
	} else {
		tm_raise("apply: expect list arguments or None, but see %o", args);
        return NONE_OBJECT;
	}
    return callFunction(func);
}

Object bfWrite() {
	Object fmt = arg_get_obj("puts");
	Object str = tmStr(fmt);
    char* s = GET_STR(str);
    int len = GET_STR_LEN(str);
    int i;
    for(i = 0; i < len; i++) {
        tm_putchar(s[i]);
    }
    return NONE_OBJECT;
}

Object bfPow() {
    double base = arg_get_double("pow");
    double y = arg_get_double("pow");
    return tm_number(pow(base, y));
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
        data->cur_obj = tm_number(cur);
        return &data->cur_obj;
    } else if (data->inc < 0 && cur > data->stop) {
        data->cur += data->inc;
        data->cur_obj = tm_number(cur);
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
		end = (long)arg_get_double(szFunc);
		inc = 1;
		break;
	case 2:
		start = (long)arg_get_double(szFunc);
		end = (long)arg_get_double(szFunc);
		inc = 1;
		break;
	case 3:
		start = (long)arg_get_double(szFunc);
		end = (long)arg_get_double(szFunc);
		inc = (long)arg_get_double(szFunc);
		break;
	default:
		tm_raise("range([n, [ n, [n]]]), but see %d arguments",
				tm->arg_cnt);
	}
	if (inc == 0)
		tm_raise("range(): increment can not be 0!");
	if (inc * (end - start) < 0)
		tm_raise("range(%d, %d, %d): not valid range!", start, end, inc);
    Object data = dataNew(sizeof(RangeIter));
    RangeIter *iterator = (RangeIter*) GET_DATA(data);
	iterator->cur = start;
	iterator->stop = end;
    iterator->inc = inc;
	iterator->proto = getRangeIterProto();
    return data;
}

Object bf_mmatch() {
    char* str = arg_get_sz("mmatch");
    int start = arg_get_int("mmatch");
    Object o_dst = arg_get_str("mmatch");
    char* dst = GET_STR(o_dst);
    int size = GET_STR_LEN(o_dst);
    return tm_number(strncmp(str+start, dst, size) == 0);
}

void builtin_funcs_init() {
	regBuiltinFunc("load", bfLoad);
	regBuiltinFunc("save", bfSave);
    regBuiltinFunc("remove", bfRemove);
	regBuiltinFunc("print", bf_print);
	regBuiltinFunc("write", bfWrite);
	regBuiltinFunc("input", bf_input);
	regBuiltinFunc("str", bf_str);
	regBuiltinFunc("int", bf_int);
	regBuiltinFunc("float", bf_float);
	regBuiltinFunc("load_module", bf_load_module);
	regBuiltinFunc("globals", bf_globals);
	regBuiltinFunc("len", bf_len);
	regBuiltinFunc("exit", bf_exit);
	regBuiltinFunc("gettype", bf_gettype);
	regBuiltinFunc("chr", bf_chr);
	regBuiltinFunc("ord", bf_ord);
	regBuiltinFunc("code8", bf_code8);
	regBuiltinFunc("code16", bf_code16);
	regBuiltinFunc("code32", bf_code32);
	regBuiltinFunc("raise", bf_raise);
	regBuiltinFunc("system", bf_system);
	regBuiltinFunc("apply", bf_apply);
    regBuiltinFunc("pow", bfPow);
    regBuiltinFunc("range", bfRange);
    regBuiltinFunc("mmatch", bf_mmatch);
}

