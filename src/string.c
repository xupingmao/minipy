#include "include/tm.h"


/* code two bytes to string, Big-Endian */
void code16(unsigned char* src, int value) {
    src[0] = (value >> 8) & 0xff;
    src[1] = (value) & 0xff;
}
int uncode16(unsigned char** src) {
    unsigned char* s = *src;
    *src += 2;
    return (s[0] << 8) + (s[1] & 0xff);
}

void code32(unsigned char* src, int value) {
    src[0] = (value >> 20) & 0xff;
    src[1] = (value >> 16) & 0xff;
    src[2] = (value >> 8) & 0xff;
    src[3] = (value) & 0xff;
}
int uncode32(unsigned char** src) {
    unsigned char* s = *src;
    *src += 4;
    return (s[0]<<20) + (s[1]<<16) + (s[2]<<8) + (s[3]);
}
/* encode/decode end */

Object string_char_new(int c) {
    String* str = tm_malloc(sizeof(String));
    struct Object obj;
    str->stype = 2; // marked as char type;
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
    return gc_track(v);
}

Object string_chr(int n) {
    return list_get(GET_LIST(ARRAY_CHARS), n);
}

void string_free(String *str) {
    if (str->stype) {
        tm_free(str->value, str->len + 1);
    }
    tm_free(str, sizeof(String));
}

int string_index(String* s1, String* s2, int start) {
    char* ss1 = s1->value;
    char* ss2 = s2->value;
    char* p = strstr(ss1 + start, ss2);
    if (p == NULL)
        return -1;
    return p - ss1;
}


/**
 * the caller always starts with str = string_new("");
 * which is not a string_chr();
 * so we can just change the str->value;
 */
Object string_append_char(Object string, char c) {
    // return obj_add(string, string_chr(c));
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
Object string_append_sz(Object string, char* sz) {
    // return obj_add(string, string_new(sz));
    String* str = GET_STR_OBJ(string);
    int sz_len = strlen(sz);
    if (str->stype) {
        str->value = tm_realloc(str->value, str->len+1, str->len+1+sz_len);
    } else {
        char* old_value = str->value;
        str->value = tm_malloc(str->len + sz_len+1);
        strcpy(str->value, old_value);
    }
    strcpy(str->value+str->len, sz);
    str->len += sz_len;
    str->stype = 1;
    return string;
}

Object string_append_obj(Object string, Object obj) {
    Object obj_str = tm_str(obj);
    char* sz = GET_STR(obj_str);
    return string_append_sz(string, sz);
}

Object string_append_int(Object string, int64_t num) {
    // the maxium length of long is 20
    char buf[30];
    sprintf(buf, "%ld", num);
    string = string_append_sz(string, buf); 
    // not use tail-call, prevent buf destroyed by compiler
    return string;
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
        return sz_to_string("");
    new_str = string_alloc(NULL, len);
    s = GET_STR(new_str);
    for (i = start; i < end; i++) {
        *(s++) = str->value[i];
    }
    return new_str;
}

Object string_builtin_find() {
    static const char* sz_func = "find";
    Object self = arg_take_str_obj(sz_func);
    Object str = arg_take_str_obj(sz_func);
    return tm_number(string_index(self.value.str, str.value.str, 0));
}

Object string_builtin_substring() {
    static const char* sz_func = "substring";
    Object self = arg_take_str_obj(sz_func);
    int start = arg_take_int(sz_func);
    int end = arg_take_int(sz_func);
    return string_substring(self.value.str, start, end);
}

Object string_builtin_upper() {
    Object self = arg_take_str_obj("upper");
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

Object string_builtin_lower() {
    Object self = arg_take_str_obj("lower");
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


Object string_builtin_replace() {
    static const char* sz_func;
    Object self = arg_take_str_obj(sz_func);
    Object src = arg_take_str_obj(sz_func);
    Object des = arg_take_str_obj(sz_func);

    Object nstr = string_alloc("", 0);
    int pos = string_index(self.value.str, src.value.str, 0);
    int lastpos = 0;
    while (pos != -1 && pos < GET_STR_LEN(self)) {
        if (pos != 0){
            nstr = obj_add(nstr,
                    string_substring(self.value.str, lastpos, pos));
        }
        nstr = obj_add(nstr, des);
        lastpos = pos + GET_STR_LEN(src);
        pos = string_index(self.value.str, src.value.str, lastpos);
        // printf("lastpos = %d\n", lastpos);
    }
    nstr = obj_add(nstr, string_substring(self.value.str, lastpos, GET_STR_LEN(self)));
    return nstr;
}

Object string_builtin_split() {
    const char* sz_func = "split";
    Object self = arg_take_str_obj(sz_func);
    Object pattern = arg_take_str_obj(sz_func);
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
            obj_append(list, string_alloc("", -1));
        } else {
            Object str = string_substring(self.value.str, lastpos, pos);
            obj_append(list, str);
        }
        lastpos = pos + GET_STR_LEN(pattern);
        pos = string_index(self.value.str, pattern.value.str, lastpos);
    }
    obj_append(list, string_substring(self.value.str, lastpos, GET_STR_LEN(self)));
    return list;
}

Object string_builtin_startswith() {
    Object self = arg_take_str_obj("str.startswith");
    Object arg0 = arg_take_str_obj("str.startswith");
    return tm_number(string_index(GET_STR_OBJ(self), GET_STR_OBJ(arg0), 0) == 0);
}

Object string_builtin_endswith() {
    const char* func_name = "str.endswith";
    Object self = arg_take_str_obj(func_name);
    Object arg0 = arg_take_str_obj(func_name);
    int idx = string_index(GET_STR_OBJ(self), GET_STR_OBJ(arg0), 0);
    if (idx < 0) {
        return tm_number(0);
    }
    return tm_number(idx + tm_len(arg0) == tm_len(self));
}

Object string_builtin_format() {
    const char* func_name = "str.format";
    Object self = arg_take_str_obj(func_name);
    Object fmt  = arg_take_str_obj(func_name);
    Object nstr = string_alloc("", 0);
    int i = 0;
    int start   = 0;
    for (i = 0; i < self.value.str->len; i++) {
        char c = self.value.str->value[i];
        if (c == '}') {
            if (start == 1) {
                Object obj = arg_take_obj(func_name);
                string_append_obj(nstr, obj);
                start = 0;
            } else {
                string_append_char(nstr, c);
            }
        } else if (c == '{') {
            start = 1;
        } else {
            start = 0;
            string_append_char(nstr, c);
        }
    }
    return nstr;
}

void string_methods_init() {
    tm->str_proto = dict_new();
    reg_mod_func(tm->str_proto, "replace",    string_builtin_replace);
    reg_mod_func(tm->str_proto, "find",       string_builtin_find);
    reg_mod_func(tm->str_proto, "substring",  string_builtin_substring);
    reg_mod_func(tm->str_proto, "upper",      string_builtin_upper);
    reg_mod_func(tm->str_proto, "lower",      string_builtin_lower);
    reg_mod_func(tm->str_proto, "split",      string_builtin_split);
    reg_mod_func(tm->str_proto, "startswith", string_builtin_startswith);
    reg_mod_func(tm->str_proto, "endswith",   string_builtin_endswith);
    reg_mod_func(tm->str_proto, "format",     string_builtin_format);
}

Object* string_next(TmData* iterator) {
    if (iterator->cur >= iterator->end) {
        return NULL;
    }
    iterator->cur += 1;
    String* str = GET_STR_OBJ(iterator->data_ptr[0]);
    iterator->cur_obj = string_chr(str->value[iterator->cur-1]);
    return &iterator->cur_obj;
}


Object string_iter_new(Object str) {
    Object data = data_new(1);
    TmData* iterator = GET_DATA(data);
    iterator->cur  = 0;
    iterator->end  = GET_STR_LEN(str);
    iterator->next = string_next;
    iterator->data_ptr[0] = str;
    return data;
}

