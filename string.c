#include "include/tm.h"
#include <ctype.h>

Object stringCharNew(int c) {
    String* str = tm_malloc(sizeof(String));
    struct Object obj;
    str->stype = 1;
    str->value = tm_malloc(2);
    str->len = 1;
    str->value[0] = c;
    str->value[1] = '\0';
    TM_TYPE(obj) = TYPE_STR;
    GET_STR_OBJ(obj) = str;
    return gcTrack(obj);
}

Object stringAlloc(char *s, int size) {
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
        return stringChr(s[0]);
    /* use string ptr in C data section */
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

Object stringChr(int n) {
    return listGet(GET_LIST(ARRAY_CHARS), n);
}

void stringFree(String *str) {
    if (str->stype) {
        tm_free(str->value, str->len + 1);
    }
    tm_free(str, sizeof(String));
}

int stringIndex(String* s1, String* s2, int start) {
    char* ss1 = s1->value;
    char* ss2 = s2->value;
    char* p = strstr(ss1 + start, ss2);
    if (p == NULL)
        return -1;
    return p - ss1;
}


Object stringSubstring(String* str, int start, int end) {
    int max_end, len, i;
    char* s;
    Object new_str;
    start = start >= 0 ? start : start + str->len;
    end = end >= 0 ? end : end + str->len;
    max_end = str->len;
    end = max_end < end ? max_end : end;
    len = end - start;
    if (len <= 0)
        return szToString("");
    new_str = stringAlloc(NULL, len);
    s = GET_STR(new_str);
    for (i = start; i < end; i++) {
        *(s++) = str->value[i];
    }
    return new_str;
}

Object string_find() {
    static const char* szFunc = "find";
    Object self = argTakeStrObj(szFunc);
    Object str = argTakeStrObj(szFunc);
    return tmNumber(stringIndex(self.value.str, str.value.str, 0));
}

Object string_substring() {
    static const char* szFunc = "substring";
    Object self = argTakeStrObj(szFunc);
    int start = argTakeInt(szFunc);
    int end = argTakeInt(szFunc);
    return stringSubstring(self.value.str, start, end);
}

Object string_upper() {
    Object self = argTakeStrObj("upper");
    int i;
    char*s = GET_STR(self);
    int len = GET_STR_LEN(self);
    Object nstr = stringAlloc(NULL, len);
    char*news = GET_STR(nstr);
    for (i = 0; i < len; i++) {
        news[i] = toupper(s[i]);
    }
    return nstr;
}

Object string_lower() {
    Object self = argTakeStrObj("lower");
    int i;
    char*s = GET_STR(self);
    int len = GET_STR_LEN(self);
    Object nstr = stringAlloc(NULL, len);
    char*news = GET_STR(nstr);
    for (i = 0; i < len; i++) {
        news[i] = tolower(s[i]);
    }
    return nstr;
}


Object string_replace() {
    static const char* szFunc;
    Object self = argTakeStrObj(szFunc);
    Object src = argTakeStrObj(szFunc);
    Object des = argTakeStrObj(szFunc);

    Object nstr = stringAlloc("", 0);
    int pos = stringIndex(self.value.str, src.value.str, 0);
    int lastpos = 0;
    while (pos != -1 && pos < GET_STR_LEN(self)) {
        if (pos != 0){
            nstr = objAdd(nstr,
                    stringSubstring(self.value.str, lastpos, pos));
        }
        nstr = objAdd(nstr, des);
        lastpos = pos + GET_STR_LEN(src);
        pos = stringIndex(self.value.str, src.value.str, lastpos);
        // printf("lastpos = %d\n", lastpos);
    }
    nstr = objAdd(nstr, stringSubstring(self.value.str, lastpos, GET_STR_LEN(self)));
    return nstr;
}

Object string_split() {
    const char* szFunc = "split";
    Object self = argTakeStrObj(szFunc);
    Object pattern = argTakeStrObj(szFunc);
    int pos, lastpos;
    Object nstr, list;
    if (GET_STR_LEN(pattern) == 0) {
        /* currently return none */
        return NONE_OBJECT;
    }
    pos = stringIndex(self.value.str, pattern.value.str, 0);
    lastpos = 0;
    nstr = stringAlloc("", 0);
    list = listNew(10);
    while (pos != -1 && pos < GET_STR_LEN(self)) {
        if (pos == 0) {
            objAppend(list, stringAlloc("", -1));
        } else {
            Object str = stringSubstring(self.value.str, lastpos, pos);
            objAppend(list, str);
        }
        lastpos = pos + GET_STR_LEN(pattern);
        pos = stringIndex(self.value.str, pattern.value.str, lastpos);
    }
    objAppend(list, stringSubstring(self.value.str, lastpos, GET_STR_LEN(self)));
    return list;
}

/**
 * the caller always starts with str = stringNew("");
 * which is not a stringChr();
 * so we can just change the str->value;
 */
Object stringAppendChar(Object string, char c) {
    // return objAdd(string, stringChr(c));
    String* str = GET_STR_OBJ(string);
    if (str->stype) {
        str->value = tm_realloc(str->value, str->len+1, str->len+2);
    } else {
        // static string, must malloc a new memory
        str->value = tm_malloc(2);
    }
    str->value[str->len] = c;
    str->value[str->len+1] = '\0';
    str->len++;
    str->stype = 1;
    return string;
}

/**
 * must be assigned
 */
Object stringAppendSz(Object string, char* sz) {
    // return objAdd(string, stringNew(sz));
    String* str = GET_STR_OBJ(string);
    int sz_len = strlen(sz);
    if (str->stype) {
        str->value = tm_realloc(str->value, str->len+1, str->len+1+sz_len);
    } else {
        char* oldValue = str->value;
        str->value = tm_malloc(str->len + sz_len+1);
        strcpy(str->value, oldValue);
    }
    strcpy(str->value+str->len, sz);
    str->len += sz_len;
    str->stype = 1;
    return string;
}

Object stringAppendObj(Object string, Object obj) {
    Object objStr = tmStr(obj);
    char* sz = GET_STR(objStr);
    return stringAppendSz(string, sz);
}

void stringMethodsInit() {
    tm->str_proto = dictNew();
    regModFunc(tm->str_proto, "replace", string_replace);
    regModFunc(tm->str_proto, "find", string_find);
    regModFunc(tm->str_proto, "substring", string_substring);
    regModFunc(tm->str_proto, "upper", string_upper);
    regModFunc(tm->str_proto, "lower", string_lower);
    regModFunc(tm->str_proto, "split", string_split);
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
    obj = stringChr(iterator->string->value[iterator->cur-1]);
    return &obj;
}
