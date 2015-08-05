#include "include/tm.h"
#include "include/vm.h"

Object newChar(int c) {
	String* str = tmMalloc(sizeof(String));
    struct Object obj;
	str->stype = 1;
	str->value = tmMalloc(2);
	str->len = 1;
	str->value[0] = c;
	str->value[1] = '\0';
	TM_TYPE(obj) = TYPE_STR;
	GET_STR_OBJ(obj) = str;
	return gcTrack(obj);
}

Object newString0(char *s, int size) {
	String* str = tmMalloc(sizeof(String));
    Object v;
    /* malloc new memory */
	if (size > 0) {
		str->stype = 1;
		str->value = tmMalloc(size + 1);
		str->len = size;
		if (s != NULL) {
			memcpy(str->value, s, size);
		} else {
			str->stype = 1;
		}
		str->value[size] = '\0';
	} else if(size == 1){
		return tmChr(s[0]);
	/* use string ptr in C stack */
	} else {
		str->stype = 0;
		if (size == 0) {
			str->value = "";
			str->len = 0;
		} else {
			str->len = strlen(s);
			str->value = s;
		}
	}
	v.type = TYPE_STR;
	v.value.str = str;
	return gcTrack(v);
}

Object tmChr(int n) {
	return ListGet(GET_LIST(ARRAY_CHARS), n);
}

void StringFree(String *str) {
#if DEBUG_GC
	int old = tm->allocate;
	printf("free string %p...\n", str);
#endif
	if (str->stype) {
		tmFree(str->value, str->len + 1);
	}
	tmFree(str, sizeof(String));
#if DEBUG_GC
	int _new = tm->allocated_mem;
	printf("free string , %d => %d, freed %d B\n", old, _new, old - _new);
#endif
}

/*
int StringEquals(String* s1, String* s2) {
	return s1->value == s2->value
			|| (s1->len == s2->len
					&& strncmp(s1->value, s2->value, s1->len) == 0);
}*/

int StringIndex(String* s1, String* s2, int start) {
	char* ss1 = s1->value;
	char* ss2 = s2->value;
	char* p = strstr(ss1 + start, ss2);
	if (p == NULL)
		return -1;
	return p - ss1;
}

int StringIndexObj(String* s1, Object value){
	tmAssertType(value, TYPE_STR, "StringIndex");
	return StringIndex(s1, GET_STR_OBJ(value), 0);
}


Object subString(String* str, int start, int end) {
    int max_end, len, i;
    char* s;
    Object new_str;
	start = start >= 0 ? start : start + str->len;
	end = end >= 0 ? end : end + str->len;
	max_end = str->len;
	end = max_end < end ? max_end : end;
	len = end - start;
	if (len <= 0)
		return staticString("");
	new_str = newString0(NULL, len);
	s = GET_STR(new_str);
	for (i = start; i < end; i++) {
		*(s++) = str->value[i];
	}
	return new_str;
}

Object bmStringFind() {
    static const char* szFunc = "find";
	Object self = getStrArg(szFunc);
	Object str = getStrArg(szFunc);
	return newNumber(StringIndex(self.value.str, str.value.str, 0));
}

Object bmSubString() {
    static const char* szFunc = "substring";
	Object self = getStrArg(szFunc);
	int start = getIntArg(szFunc);
	int end = getIntArg(szFunc);
	return subString(self.value.str, start, end);
}

Object bmStringUpper() {
	Object self = getStrArg("upper");
	int i;
	char*s = GET_STR(self);
	int len = GET_STR_LEN(self);
	Object nstr = newString0(NULL, len);
	char*news = GET_STR(nstr);
	for (i = 0; i < len; i++) {
		if (s[i] >= 'a' && s[i] <= 'z') {
			news[i] = s[i] + 'A' - 'a';
		} else {
			news[i] = s[i];
		}
	}
	return nstr;
}

Object bmStringIsUpper() {
	Object self = getStrArg("isupper");
	int i;
	for (i = 0; i < GET_STR_LEN(self); i++) {
		char c = GET_STR(self)[i];
		if (c >= 'A' && c <= 'Z') {

		} else {
			return NUMBER_FALSE;
		}
	}
	return NUMBER_TRUE;
}

Object bmStringLower() {
	Object self = getStrArg("lower");
	int i;
	char*s = GET_STR(self);
	int len = GET_STR_LEN(self);
	Object nstr = newString0(NULL, len);
	char*news = GET_STR(nstr);
	for (i = 0; i < len; i++) {
		if (s[i] >= 'A' && s[i] <= 'Z') {
			news[i] = s[i] + 'a' - 'A';
		} else {
			news[i] = s[i];
		}
	}
	return nstr;
}

Object bmStringReplace() {
    static const char* szFunc;
	Object self = getStrArg(szFunc);
	Object src = getStrArg(szFunc);
	Object des = getStrArg(szFunc);

	Object nstr = newString0("", 0);
	int pos = StringIndex(self.value.str, src.value.str, 0);
	int lastpos = 0;
	while (pos != -1 && pos < GET_STR_LEN(self)) {
		if (pos != 0){
			nstr = tmAdd(nstr,
					subString(self.value.str, lastpos, pos));
		}
		nstr = tmAdd(nstr, des);
		lastpos = pos + GET_STR_LEN(src);
		pos = StringIndex(self.value.str, src.value.str, lastpos);
		// printf("lastpos = %d\n", lastpos);
	}
	nstr = tmAdd(nstr, subString(self.value.str, lastpos, GET_STR_LEN(self)));
	return nstr;
}

Object bmStringSplit() {
	const char* szFunc = "split";
	Object self = getStrArg(szFunc);
	Object pattern = getStrArg(szFunc);
    int pos, lastpos;
    Object nstr, list;
	if (GET_STR_LEN(pattern) == 0) {
		/* currently return none */
		return NONE_OBJECT;
	}
	pos = StringIndex(self.value.str, pattern.value.str, 0);
	lastpos = 0;
	nstr = newString0("", 0);
	list = newList(10);
	while (pos != -1 && pos < GET_STR_LEN(self)) {
		if (pos == 0) {
			_listAppend(GET_LIST(list), newString0("", -1));
		} else {
			Object str = subString(self.value.str, lastpos, pos);
			_listAppend(GET_LIST(list), str);
		}
		lastpos = pos + GET_STR_LEN(pattern);
		pos = StringIndex(self.value.str, pattern.value.str, lastpos);
	}
	_listAppend(GET_LIST(list), subString(self.value.str, lastpos, GET_STR_LEN(self)));
	return list;
}

Object StringJoin(Object self, Object list) {
	Object str = newString0("", 0);
	int i = 0;
	for (i = 0; i < LIST_LEN(list); i++) {
		Object s = LIST_NODES(list)[i];
		tmAssertType(s, TYPE_STR, "string.join");
		if (i != 0)
			str = tmAdd(str, self);
		str = tmAdd(str, s);
	}
	return str;
}

Object bmStringJoin() {
    static const char* szFunc = "join";
	Object self = getStrArg(szFunc);
	Object list = getListArg(szFunc);
	return StringJoin(self, list);
}

Object bmStringFormat() {
	const char* szFunc = "string.format";
	Object str = getStrArg(szFunc);
	char* fmt = GET_STR(str);
	StringBuilder *sb = StringBuilderNew();
	int i = 0, j = 1;
	while (i < GET_STR_LEN(str)) {
		char c = fmt[i];
		if (c == '%' && fmt[i + 1] == 's') {
            Object obj = getObjArg(szFunc);
			StringBuilderAppendObj(sb, obj);
			i++;j++;
		} else {
			StringBuilderAppend(sb, fmt[i]);
		}
		i += 1;
	}
	return StringBuilderToStr(sb);
}


void regStringMethods() {
	CLASS_STRING = newDict();
	regConst(CLASS_STRING);
	regModFunc(CLASS_STRING, "replace", bmStringReplace);
	regModFunc(CLASS_STRING, "find", bmStringFind);
	regModFunc(CLASS_STRING, "substring", bmSubString);
	regModFunc(CLASS_STRING, "upper", bmStringUpper);
	regModFunc(CLASS_STRING, "lower", bmStringLower);
	regModFunc(CLASS_STRING, "split", bmStringSplit);
	regModFunc(CLASS_STRING, "join", bmStringJoin);
	regModFunc(CLASS_STRING, "format", bmStringFormat);
}

DataProto* getStringProto() {
	if(!stringProto.init) {
		initDataProto(&stringProto);
		stringProto.next = stringNext;
		stringProto.dataSize = sizeof(StringIterator);
	}
	return &stringProto;
}

Object stringIterNew(String* str) {
	Object data = dataNew(sizeof(StringIterator));
	StringIterator* iterator = (StringIterator*) GET_DATA(data);
	iterator->cur = 0;
	iterator->string = str;
	iterator->proto = getStringProto();
	return data;
}

Object* stringNext(StringIterator* iterator) {
    static Object obj;
	if (iterator->cur >= iterator->string->len) {
		return NULL;
	}
	iterator->cur += 1;
	obj = tmChr(iterator->string->value[iterator->cur-1]);
    return &obj;
}
