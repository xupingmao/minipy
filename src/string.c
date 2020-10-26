/**
 * description here
 * @author xupingmao
 * @since 2018/02/19 16:49:28
 * @modified 2020/10/21 01:18:05
 */

#include "include/mp.h"

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

/**
 * allocate a new string of length 1
 * @since 2015
 */
MpObj string_char_new(int c) {
    MpStr* str = mp_malloc(sizeof(MpStr));
    struct MpObj obj;
    str->stype = 2; // marked as char type;
    str->value = mp_malloc(2);
    str->len = 1;
    str->value[0] = c;
    str->value[1] = '\0';
    MP_TYPE(obj) = TYPE_STR;
    GET_STR_OBJ(obj) = str;
    return gc_track(obj);
}

/**
 * allocate new string
 * use constant char if size <= 0
 */
MpObj string_alloc(char *s, int size) {
    MpStr* str = mp_malloc(sizeof(MpStr));
    MpObj v;
    if (size > 0) {
        /* copy string data to new memory */
        str->stype = 1;
        str->value = mp_malloc(size + 1);
        str->len = size;
        if (s != NULL) {
            memcpy(str->value, s, size);
        } else {
            str->stype = 1;
        }
        str->value[size] = '\0';
    } else if(size == 1){
        return string_chr(s[0]);
    } else {
        /* use string ptr in C data section */
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

MpObj string_new(char* s) {
    return string_alloc(s, strlen(s));
}

/**
 * create a new string and put it to tm->constants
 * this object will not be tracked by gc
 * @since 2016-08-22
 */
MpObj string_const(char* s) {
    MpObj str_obj = string_new(s);
    int i = dict_set(tm->constants, str_obj, NONE_OBJECT);
    return DICT_NODES(tm->constants)[i].key;
}


/**
 * @since 2016-11-27
 */
MpObj string_const2(char* s, int len) {
    MpObj str_obj = string_alloc(s, len);
    int i = dict_set(tm->constants, str_obj, NONE_OBJECT);
    return DICT_NODES(tm->constants)[i].key;
}

MpObj string_chr(int n) {
    // use static str_pool to optimize
    // TODO, need handle GC problems
    return list_get(GET_LIST(ARRAY_CHARS), n);
}

void string_free(MpStr *str) {
    if (str->stype) {
        mp_free(str->value, str->len + 1);
    }
    mp_free(str, sizeof(MpStr));
}

int string_index(MpStr* s1, MpStr* s2, int start) {
    char* ss1 = s1->value;
    char* ss2 = s2->value;
    // char* p = strstr(ss1 + start, ss2);
    // strstr can not handle \0
    int i = 0;
    int len = s1->len - s2->len;
    for (i = start; i <= len; i++) {
        if (memcmp(ss1+i, ss2, s2->len) == 0) {
            return i;
        }
    }
    return -1;
}


int string_rfind(MpStr* s1, MpStr* s2) {
    char* ss1 = s1->value;
    char* ss2 = s2->value;

    int end = s1->len - s2->len;
    int i = end - 1;

    if (end < 0) {
        return -1;
    }

    for (i = end; i >= 0; i--) {
        if (memcmp(ss1+i, ss2, s2->len) == 0) {
            return i;
        }
    }

    return -1;
}

/**
 * the caller always starts with str = string_new("");
 * which is not a string_chr();
 * so we can just change the str->value;
 */
MpObj string_append_char(MpObj string, char c) {
    // return obj_add(string, string_chr(c));
    MpStr* str = GET_STR_OBJ(string);
    if (str->stype) {
        str->value = mp_realloc(str->value, str->len+1, str->len+2);
    } else {
        // static string, must malloc a new memory
        str->value = mp_malloc(str->len+2);
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
MpObj string_append_sz(MpObj string, char* sz) {
    // return obj_add(string, string_new(sz));
    MpStr* str = GET_STR_OBJ(string);
    int sz_len = strlen(sz);
    if (str->stype) {
        str->value = mp_realloc(str->value, str->len+1, str->len+1+sz_len);
    } else {
        char* old_value = str->value;
        str->value = mp_malloc(str->len + sz_len+1);
        strcpy(str->value, old_value);
    }
    strcpy(str->value+str->len, sz);
    str->len += sz_len;
    str->stype = 1;
    return string;
}

MpObj string_append_obj(MpObj string, MpObj obj) {
    MpObj obj_str = mp_str(obj);
    char* sz = GET_STR(obj_str);
    return string_append_sz(string, sz);
}

MpObj string_append_int(MpObj string, int64_t num) {
    // the maxium length of long is 20
    char buf[30];
    sprintf(buf, "%lld", (long long int)num);
    string = string_append_sz(string, buf); 
    // not use tail-call, prevent buf destroyed by compiler
    return string;
}


MpObj string_substring(MpStr* str, int start, int end) {
    int max_end, len, i;
    char* s;
    MpObj new_str;
    start = start >= 0 ? start : start + str->len;
    end = end >= 0 ? end : end + str->len;
    max_end = str->len;
    end = max_end < end ? max_end : end;
    len = end - start;
    if (len <= 0)
        return string_from_sz("");
    new_str = string_alloc(NULL, len);
    s = GET_STR(new_str);
    for (i = start; i < end; i++) {
        *(s++) = str->value[i];
    }
    return new_str;
}

MpObj string_builtin_find() {
    static const char* sz_func = "find";
    MpObj self = arg_take_str_obj(sz_func);
    MpObj str = arg_take_str_obj(sz_func);
    return number_obj(string_index(self.value.str, str.value.str, 0));
}

MpObj string_builtin_rfind() {
    MpObj self = arg_take_str_obj("rfind");
    MpObj  str = arg_take_str_obj("rfind");
    return number_obj(string_rfind(self.value.str, str.value.str));
}

MpObj string_builtin_substring() {
    static const char* sz_func = "substring";
    MpObj self = arg_take_str_obj(sz_func);
    int start = arg_take_int(sz_func);
    int end = arg_take_int(sz_func);
    return string_substring(self.value.str, start, end);
}

MpObj string_builtin_upper() {
    MpObj self = arg_take_str_obj("upper");
    int i;
    char*s = GET_STR(self);
    int len = GET_STR_LEN(self);
    MpObj nstr = string_alloc(NULL, len);
    char*news = GET_STR(nstr);
    for (i = 0; i < len; i++) {
        news[i] = toupper(s[i]);
    }
    return nstr;
}

MpObj string_builtin_lower() {
    MpObj self = arg_take_str_obj("lower");
    int i;
    char*s = GET_STR(self);
    int len = GET_STR_LEN(self);
    MpObj nstr = string_alloc(NULL, len);
    char*news = GET_STR(nstr);
    for (i = 0; i < len; i++) {
        news[i] = tolower(s[i]);
    }
    return nstr;
}


MpObj string_builtin_replace() {
    static const char* sz_func;
    MpObj self = arg_take_str_obj(sz_func);
    MpObj src = arg_take_str_obj(sz_func);
    MpObj des = arg_take_str_obj(sz_func);

    MpObj nstr = string_alloc("", 0);
    int pos = string_index(self.value.str, src.value.str, 0);
    int lastpos = 0;
    while (pos >=0 && pos < GET_STR_LEN(self)) {
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

MpObj string_builtin_split() {
    const char* sz_func = "split";
    MpObj self    = arg_take_str_obj(sz_func);
    MpObj pattern = arg_take_str_obj(sz_func);
    int pos, lastpos;
    MpObj nstr, list;
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
            MpObj str = string_substring(self.value.str, lastpos, pos);
            obj_append(list, str);
        }
        lastpos = pos + GET_STR_LEN(pattern);
        pos = string_index(self.value.str, pattern.value.str, lastpos);
    }
    obj_append(list, string_substring(self.value.str, lastpos, GET_STR_LEN(self)));
    return list;
}

MpObj string_builtin_startswith() {
    MpObj self = arg_take_str_obj("str.startswith");
    MpObj arg0 = arg_take_str_obj("str.startswith");
    return number_obj(string_index(GET_STR_OBJ(self), GET_STR_OBJ(arg0), 0) == 0);
}

MpObj string_builtin_endswith() {
    const char* func_name = "str.endswith";
    MpObj self = arg_take_str_obj(func_name);
    MpObj arg0 = arg_take_str_obj(func_name);
    int idx = string_index(GET_STR_OBJ(self), GET_STR_OBJ(arg0), 0);
    if (idx < 0) {
        return number_obj(0);
    }
    return number_obj(idx + mp_len(arg0) == mp_len(self));
}

MpObj string_builtin_format() {
    const char* func_name = "str.format";
    MpObj self = arg_take_str_obj(func_name);
    // MpObj fmt  = arg_take_str_obj(func_name);
    MpObj nstr = string_alloc("", 0);
    int i = 0;
    int start   = 0;
    for (i = 0; i < self.value.str->len; i++) {
        char c = self.value.str->value[i];
        if (c == '}') {
            if (start == 1) {
                MpObj obj = arg_take_obj(func_name);
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

    if (start == 1) {
        string_append_char(nstr, '{');
    }
    
    return nstr;
}

void string_methods_init() {
    tm->str_proto = dict_new();
    reg_mod_func(tm->str_proto, "replace",    string_builtin_replace);
    reg_mod_func(tm->str_proto, "find",       string_builtin_find);
    reg_mod_func(tm->str_proto, "rfind",      string_builtin_rfind);
    reg_mod_func(tm->str_proto, "substring",  string_builtin_substring);
    reg_mod_func(tm->str_proto, "upper",      string_builtin_upper);
    reg_mod_func(tm->str_proto, "lower",      string_builtin_lower);
    reg_mod_func(tm->str_proto, "split",      string_builtin_split);
    reg_mod_func(tm->str_proto, "startswith", string_builtin_startswith);
    reg_mod_func(tm->str_proto, "endswith",   string_builtin_endswith);
    reg_mod_func(tm->str_proto, "format",     string_builtin_format);
}

MpObj* string_next(MpData* iterator) {
    if (iterator->cur >= iterator->end) {
        return NULL;
    }
    iterator->cur += 1;
    MpStr* str = GET_STR_OBJ(iterator->data_ptr[0]);
    iterator->cur_obj = string_chr(str->value[iterator->cur-1]);
    return &iterator->cur_obj;
}


MpObj string_iter_new(MpObj str) {
    MpObj data = data_new(1);
    MpData* iterator = GET_DATA(data);
    iterator->cur  = 0;
    iterator->end  = GET_STR_LEN(str);
    iterator->next = string_next;
    iterator->data_ptr[0] = str;
    return data;
}

