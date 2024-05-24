/**
 * description here
 * @author xupingmao
 * @since 2018/02/19 16:49:28
 * @modified 2022/06/10 22:59:13
 */

#include "include/mp.h"
#include "include/gc_debug.h"

static MpObj string_rstrip_chars(MpStr* self, char* chars, int chars_len);
static void string_update_hash(MpStr* str);
MpObj string_to_obj(MpStr* str);

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
    assert (c>=0);
    assert (c<=255);
    char value[2];
    value[0] = c;
    value[1] = 0;
    return string_alloc(value, 1);
}

/**
 * allocate new string
 * use constant char if size <= 0
 */
MpObj string_alloc(char *s, int size) {
    MpStr* str = mp_malloc(sizeof(MpStr), "str.alloc_obj");
    if (size > 0) {
        /* copy string data to new memory */
        str->stype = STR_TYPE_DYNAMIC;
        str->value = mp_malloc(size + 1, "str.alloc_value");
        str->len = size;
        if (s != NULL) {
            memcpy(str->value, s, size);
        } else {
            /* if `s` is NULL, just allocate memory */
        }
        str->value[size] = '\0';
        gc_debug_str_value(str->value);
    } else if(size == 1){
        return string_chr(s[0]);
    } else {
        /* use string ptr in C data section */
        str->stype = STR_TYPE_STATIC;
        if (size == 0) {
            str->value = "";
            str->len = 0;
        } else {
            str->len = strlen(s);
            str->value = s;
        }
    }

    MpObj v = string_to_obj(str); 
    string_update_hash(str);
    return gc_track(v);
}

MpObj string_new(char* s) {
    return string_alloc(s, strlen(s));
}

MpObj string_to_obj(MpStr* str) {
    MpObj o;
    o.type = TYPE_STR;
    o.value.str = str;
    return o;
}

MpObj string_static(const char* s) {
    return string_from_cstr(s);
}

MpObj string_from_cstr(const char* s) {
    return string_alloc((char*)s, -1);
}

static void string_update_hash(MpStr* s) {
    s->hash = mp_hash(s->value, s->len);
}

/**
 * create a new string and put it to tm->constants
 * this object will not be tracked by gc
 * @since 2016-08-22
 */
MpObj string_const(const char* s) {
    return string_from_cstr(s);
}


MpObj string_intern(const char* s) {
    return string_from_cstr(s);
}


/**
 * @since 2016-11-27
 */
MpObj string_const_with_len(char* s, int len) {
    MpObj str_obj = string_alloc(s, len);
    int i = dict_set0(tm->constants, str_obj, NONE_OBJECT);
    return tm->constants->nodes[i].key;
}

MpObj string_chr(int n) {
    // use static str_pool to optimize
    // TODO, need handle GC problems
    return list_get(GET_LIST(ARRAY_CHARS), n);
}

void string_free(MpStr *str) {
    if (str->stype == STR_TYPE_DYNAMIC) {
        mp_free(str->value, str->len + 1);
        mp_free(str, sizeof(MpStr));
    } else if (str->stype == STR_TYPE_STATIC) {
        mp_free(str, sizeof(MpStr));
    } else {
        mp_raise("string_free: unknown stype: %d", str->stype);
    }
}

int string_index(MpStr* s1, MpStr* s2, int start) {
    assert(s1 != NULL);
    assert(s2 != NULL);

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

static int string_size(MpStr* s) {
    return s->len;
}

int string_hash(MpStr* s) {
    if (s->hash < 0) {
        s->hash = mp_hash(s->value, s->len);
    } 
    return s->hash;
}

static char string_char_at(MpStr* s, int index) {
    if (index < 0) {
        index += string_size(s);
    }
    if (index < 0) {
        return 0;
    }
    if (index >= string_size(s)) {
        return 0;
    }
    return s->value[index];
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
        str->value = mp_realloc(str->value, str->len+1, str->len+2, "str.append_char");
    } else {
        // static string, must malloc a new memory
        str->value = mp_malloc(str->len+2, "str.append_char");
    }
    str->value[str->len] = c;
    str->value[str->len+1] = '\0';
    str->len++;
    str->stype = 1;
    
    string_update_hash(str);
    return string;
}

/**
 * must be assigned
 */
MpObj string_append_cstr(MpObj string, const char* sz) {
    // return obj_add(string, string_new(sz));
    MpStr* str = GET_STR_OBJ(string);
    int sz_len = strlen(sz);
    if (str->stype) {
        str->value = mp_realloc(str->value, str->len+1, str->len+1+sz_len, "str.append_cstr");
    } else {
        char* old_value = str->value;
        str->value = mp_malloc(str->len + sz_len+1, "str.append_cstr");
        strcpy(str->value, old_value);
    }
    strcpy(str->value+str->len, sz);
    str->len += sz_len;
    str->stype = 1;

    string_update_hash(str);
    return string;
}

MpObj string_append_obj(MpObj string, MpObj obj) {
    MpObj str = obj_str(obj);
    char* sz = GET_CSTR(str);
    return string_append_cstr(string, sz);
}

MpObj string_append_int(MpObj string, int64_t num) {
    // the maxium length of long is 20
    char buf[30];
    sprintf(buf, "%lld", (long long int)num);
    string = string_append_cstr(string, buf); 
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
    if (len <= 0) {
        return string_from_cstr("");
    }

    new_str = string_alloc(NULL, len);
    s = GET_CSTR(new_str);
    for (i = start; i < end; i++) {
        *(s++) = str->value[i];
    }

    string_update_hash(GET_STR_OBJ(new_str));
    return new_str;
}

MpObj string_add(MpStr* a, MpStr* b) {
    char* sa = a->value;
    char* sb = b->value;
    int la = a->len;
    int lb = b->len;
    if (la == 0) {
        return string_to_obj(b);
    }
    if (lb == 0) {
        return string_to_obj(a);
    }

    int len = la + lb;
    MpObj des = string_alloc(NULL, len);
    char*s = GET_CSTR(des);
    memcpy(s, sa, la);
    memcpy(s + la, sb, lb);
    string_update_hash(GET_STR_OBJ(des));
    return des;
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
    char*s = GET_CSTR(self);
    int len = GET_STR_LEN(self);
    MpObj nstr = string_alloc(NULL, len);
    char*news = GET_CSTR(nstr);
    for (i = 0; i < len; i++) {
        news[i] = toupper(s[i]);
    }
    string_update_hash(GET_STR_OBJ(nstr));
    return nstr;
}

MpObj string_builtin_lower() {
    MpObj self = arg_take_str_obj("lower");
    int i;
    char*s = GET_CSTR(self);
    int len = GET_STR_LEN(self);
    MpObj nstr = string_alloc(NULL, len);
    char*news = GET_CSTR(nstr);
    for (i = 0; i < len; i++) {
        news[i] = tolower(s[i]);
    }
    string_update_hash(GET_STR_OBJ(nstr));
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
    MpObj nstr = string_alloc("", 0);
    int i = 0;

    /* mark `{` */
    int start = 0;
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
            if (start == 1) {
                // 转义字符
                string_append_char(nstr, c);
                start = 0;
            } else {
                start = 1;
            }
        } else {
            start = 0;
            string_append_char(nstr, c);
        }
    }

    if (start == 1) {
        // string_append_char(nstr, '{');
        mp_raise("ValueError: Single '{' encountered in format string");
    }
    
    return nstr;
}

static MpObj string_rstrip_blank(MpStr* self) {
    return string_rstrip_chars(self, "\t\r\n ", 4);
}

static MpObj string_rstrip_chars(MpStr* self, 
                            char* chars, 
                            int chars_len) {
    int length = 0;
    int i = 0;
    int j = 0;

    for (i = self->len - 1; i >= 0; i--) {
        char c = string_char_at(self, i);

        int match = 0;
        for (j = 0; j<chars_len; j++) {
            if (c == chars[j]) {
                match = 1;
                break;
            }
        }

        if (match) {
            length++;
        } else {
            break;
        }
    }
    char* value = self->value;
    return string_alloc(value, string_size(self) - length);   
}

MpObj string_builtin_rstrip() {
    MpObj self = arg_take_obj("str.rstrip");
    int argc = arg_count();
    if (argc == 1) {
        // strip blank chars
        return string_rstrip_blank(GET_STR_OBJ(self));
    } else {
        MpObj chars = arg_take_str_obj("str.rstrip");
        return string_rstrip_chars(GET_STR_OBJ(self), 
            GET_CSTR(chars), GET_STR_LEN(chars));
    }
}

void string_methods_init() {
    tm->str_proto = dict_new();
    mod_reg_func(tm->str_proto, "replace",    string_builtin_replace);
    mod_reg_func(tm->str_proto, "find",       string_builtin_find);
    mod_reg_func(tm->str_proto, "rfind",      string_builtin_rfind);
    mod_reg_func(tm->str_proto, "substring",  string_builtin_substring);
    mod_reg_func(tm->str_proto, "upper",      string_builtin_upper);
    mod_reg_func(tm->str_proto, "lower",      string_builtin_lower);
    mod_reg_func(tm->str_proto, "split",      string_builtin_split);
    mod_reg_func(tm->str_proto, "startswith", string_builtin_startswith);
    mod_reg_func(tm->str_proto, "endswith",   string_builtin_endswith);
    mod_reg_func(tm->str_proto, "format",     string_builtin_format);
    mod_reg_func(tm->str_proto, "rstrip",     string_builtin_rstrip);
}

MpObj* string_next(MpData* iterator) {
    assert(iterator != NULL);

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

