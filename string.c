#include "include/tm.h"
#include "include/vm.h"

Object string_char(int c) {
	String* str = tm_malloc(sizeof(String));
    struct Object obj;
	str->stype = 1;
	str->value = tm_malloc(2);
	str->len = 1;
	str->value[0] = c;
	str->value[1] = '\0';
	TM_TYPE(obj) = TYPE_STR;
	GET_STR_OBJ(obj) = str;
	return gc_track(obj);
}

Object string_alloc(char *s, int size) {
	String* str = tm_malloc(sizeof(String));
    Object v;
    /* malloc new memory */
	if (size > 0) {
		str->stype = 1;
		str->value = tm_malloc(size + 1);
		str->len = size;
		if (s != NULL) {
			memcpy(str->value, s, size);
		} else {
			str->stype = 1;
		}
		str->value[size] = '\0';
	} else if(size == 1){
		return string_chr(s[0]);
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
	return gc_track(v);
}

Object string_chr(int n) {
	return list_get(GET_LIST(ARRAY_CHARS), n);
}

void string_free(String *str) {
#if DEBUG_GC
	int old = tm->allocate;
	printf("free string %p...\n", str);
#endif
	if (str->stype) {
		tm_free(str->value, str->len + 1);
	}
	tm_free(str, sizeof(String));
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

int string_index(String* s1, String* s2, int start) {
	char* ss1 = s1->value;
	char* ss2 = s2->value;
	char* p = strstr(ss1 + start, ss2);
	if (p == NULL)
		return -1;
	return p - ss1;
}


Object string_substring(String* str, int start, int end) {
    int max_end, len, i;
    char* s;
    Object new_str;
	start = start >= 0 ? start : start + str->len;
	end = end >= 0 ? end : end + str->len;
	max_end = str->len;
	end = max_end < end ? max_end : end;
	len = end - start;
	if (len <= 0)
		return string_static("");
	new_str = string_alloc(NULL, len);
	s = GET_STR(new_str);
	for (i = start; i < end; i++) {
		*(s++) = str->value[i];
	}
	return new_str;
}

Object string_m_find() {
    static const char* szFunc = "find";
	Object self = getStrArg(szFunc);
	Object str = getStrArg(szFunc);
	return tm_number(string_index(self.value.str, str.value.str, 0));
}

Object string_m_substring() {
    static const char* szFunc = "substring";
	Object self = getStrArg(szFunc);
	int start = getIntArg(szFunc);
	int end = getIntArg(szFunc);
	return string_substring(self.value.str, start, end);
}

Object string_m_upper() {
	Object self = getStrArg("upper");
	int i;
	char*s = GET_STR(self);
	int len = GET_STR_LEN(self);
	Object nstr = string_alloc(NULL, len);
	char*news = GET_STR(nstr);
	for (i = 0; i < len; i++) {
		news[i] = toupper(s[i]);
	}
	return nstr;
}

Object string_m_lower() {
	Object self = getStrArg("lower");
	int i;
	char*s = GET_STR(self);
	int len = GET_STR_LEN(self);
	Object nstr = string_alloc(NULL, len);
	char*news = GET_STR(nstr);
	for (i = 0; i < len; i++) {
		news[i] = tolower(s[i]);
	}
	return nstr;
}


Object string_m_replace() {
    static const char* szFunc;
	Object self = getStrArg(szFunc);
	Object src = getStrArg(szFunc);
	Object des = getStrArg(szFunc);

	Object nstr = string_alloc("", 0);
	int pos = string_index(self.value.str, src.value.str, 0);
	int lastpos = 0;
	while (pos != -1 && pos < GET_STR_LEN(self)) {
		if (pos != 0){
			nstr = tm_add(nstr,
					string_substring(self.value.str, lastpos, pos));
		}
		nstr = tm_add(nstr, des);
		lastpos = pos + GET_STR_LEN(src);
		pos = string_index(self.value.str, src.value.str, lastpos);
		// printf("lastpos = %d\n", lastpos);
	}
	nstr = tm_add(nstr, string_substring(self.value.str, lastpos, GET_STR_LEN(self)));
	return nstr;
}

Object string_m_split() {
	const char* szFunc = "split";
	Object self = getStrArg(szFunc);
	Object pattern = getStrArg(szFunc);
    int pos, lastpos;
    Object nstr, list;
	if (GET_STR_LEN(pattern) == 0) {
		/* currently return none */
		return NONE_OBJECT;
	}
	pos = string_index(self.value.str, pattern.value.str, 0);
	lastpos = 0;
	nstr = string_alloc("", 0);
	list = list_new(10);
	while (pos != -1 && pos < GET_STR_LEN(self)) {
		if (pos == 0) {
			list_append(GET_LIST(list), string_alloc("", -1));
		} else {
			Object str = string_substring(self.value.str, lastpos, pos);
			list_append(GET_LIST(list), str);
		}
		lastpos = pos + GET_STR_LEN(pattern);
		pos = string_index(self.value.str, pattern.value.str, lastpos);
	}
	list_append(GET_LIST(list), string_substring(self.value.str, lastpos, GET_STR_LEN(self)));
	return list;
}

/* 
this may cause GC trash.
Object StringJoin(Object self, Object list) {
	Object str = string_alloc("", 0);
	int i = 0;
	for (i = 0; i < LIST_LEN(list); i++) {
		Object s = LIST_NODES(list)[i];
		tmAssertType(s, TYPE_STR, "string.join");
		if (i != 0)
			str = tm_add(str, self);
		str = tm_add(str, s);
	}
	return str;
}

Object bmStringJoin() {
    static const char* szFunc = "join";
	Object self = getStrArg(szFunc);
	Object list = getListArg(szFunc);
	return StringJoin(self, list);
}
*/


void string_methods_init() {
	tm->str_proto = dict_new();
	regModFunc(tm->str_proto, "replace", string_m_replace);
	regModFunc(tm->str_proto, "find", string_m_find);
	regModFunc(tm->str_proto, "substring", string_m_substring);
	regModFunc(tm->str_proto, "upper", string_m_upper);
	regModFunc(tm->str_proto, "lower", string_m_lower);
	regModFunc(tm->str_proto, "split", string_m_split);
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
	obj = string_chr(iterator->string->value[iterator->cur-1]);
    return &obj;
}
