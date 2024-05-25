/**
 * description here
 * @author xupingmao <578749341@qq.com>
 * @since 2016
 * @modified 2022/06/09 23:12:02
 */
#include "include/mp.h"
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

MpObj* mp_get_builtin(char* key) {
    if (!tm->init) {
        return NULL;
    }
    return dict_get_by_str(tm->builtins, key);
}

void mp_putchar(int c){
    static char hex[] = {
        '0','1','2','3','4','5','6','7',
        '8','9','A','B','C','D','E','F'
    };

    if (isprint(c) || c == '\n' || c == '\t') {
        putchar(c);
    } else if(c == '\b') {
        putchar('\b');
    } else if(c=='\r'){
        /* output nothing */
    } else {
        int c0 = (c & 0xf0) >> 4;
        int c1 = c & 0x0f;
        printf("0x%c%c", hex[c0], hex[c1]);
    }
}

void mp_inspect_char(int c) {
    static char hex[] = {
        '0','1','2','3','4','5','6','7',
        '8','9','A','B','C','D','E','F'
    };

    if (isprint(c)) {
        putchar(c);
    } else if (c == '\b') {
        putchar('\b');
    } else if (c == '\n') {
        printf("\\n");
    } else if (c == '\t') {
        printf("\\t");
    } else if(c=='\r'){
        /* output nothing */
    } else {
        int c0 = (c & 0xf0) >> 4;
        int c1 = c & 0x0f;
        printf("0x%c%c", hex[c0], hex[c1]);
    }
}

void mp_print(MpObj o) {
    MpObj str = obj_str(o);
    int i;
    for(i = 0; i < GET_STR_LEN(str); i++) {
        mp_putchar(GET_CSTR(str)[i]);
    }
}

void mp_println(MpObj o) {
    mp_print(o);
    puts("");
}

void mp_inspect_obj0(MpObj o, int padding) {
    int i;
    const int max_len = 20;
    while (--padding>=0) {
        printf(" ");
    }
    char buf[128];
    switch(MP_TYPE(o)) {
        case TYPE_NUM: 
            number_format(buf, o);
            printf("%s\n", buf);
            break;
        case TYPE_NONE:
            printf("None\n");
            break;
        case TYPE_STR:
            printf("<string len=%d value=", GET_STR_LEN(o));
            for (i = 0; i < GET_STR_LEN(o) && i <= max_len; i++) {
                mp_inspect_char(GET_CSTR(o)[i]);
            }
            printf(">\n");
            break;
        case TYPE_FUNCTION:
            func_format(buf, GET_FUNCTION(o));
            printf("%s\n", buf);
            break;
        case TYPE_DATA:
            printf("<data %p>\n", GET_DATA(o));
            break;
        case TYPE_DICT:
            printf("<dict len=%d %p>\n", DICT_LEN(o), GET_DICT(o));
            break;
        case TYPE_LIST:
            printf("<list len=%d %p>\n", LIST_LEN(o), GET_LIST(o));
            break;
        default:
            printf("<unknown %d>\n", MP_TYPE(o));
    }
}

void mp_inspect_obj(MpObj o) {
    int i;
    switch(MP_TYPE(o)) {
        case TYPE_LIST:
            printf("[\n");
            for (i = 0; i < LIST_LEN(o); i++) {
                mp_inspect_obj0(LIST_NODES(o)[i], 2);
            }
            printf("]\n");
            break;
        case TYPE_DICT:
            printf("{\n");
            for (i = 0; i < DICT_LEN(o); i++) {
                mp_inspect_obj0(DICT_NODES(o)[i].key, 2);
                mp_inspect_obj0(DICT_NODES(o)[i].val, 4);
            }
            printf("}\n");
            break;
        default:
            mp_inspect_obj0(o, 0);
    }
}


/**
 * based on C language standard.
 * d  -> int
 * f  -> double
 * c  -> char
 * s  -> char*
 * p  -> pointer
 * o  -> repl(object)
 * os -> object
 */
MpObj mp_format_va_list(char* fmt, va_list ap, int append_newline) {
    return mp_format_va_list_check_length(fmt, ap, -1, append_newline);
}

MpObj mp_format_va_list_check_length(char* fmt, va_list ap, int ap_length, int append_newline) {
    int i;
    int len = strlen(fmt);
    MpObj str = string_new("");
    int templ = 0;
    char* start = fmt;
    int istrans = 1;
    char buf[50];
    int args_in_fmt = 0;
    char* fmt0 = fmt;

    for (i = 0; i < len; i++) {
        if (fmt[i] == '%') {
            i++;
            if (fmt[i] == '%') {
                continue;
            }

            // format arguments
            args_in_fmt += 1;
            if (ap_length != -1 && args_in_fmt > ap_length) {
                mp_raise("TypeError: not enough arguments for format string");
            }

            char num_fmt[20];
            int  num_len = 1;
            // -1是为了把%加进取
            char* fmt_store = fmt + i - 1;
            // 重置buf
            // memset(void *s,int ch,size_t n);
            memset(num_fmt, 0, sizeof(num_fmt));

            if (fmt[i] == '-') {
                num_len++;
                i++;
            }

            for (; isdigit(fmt[i]) && i < len; i++) {
                num_len++;
            }

            if (num_len >= sizeof(num_fmt)) {
                mp_raise("mp_format_va_list(%d): Invalid format", __LINE__);
            }

            switch (fmt[i]) {
            case 'd':
                // char *strncpy(char *dest, const char *src, size_t n) 
                strncpy(num_fmt, fmt_store, num_len+1);
                sprintf(buf, num_fmt, va_arg(ap, int));
                str = string_append_cstr(str, buf);
                break;
            case 'x':
                sprintf(buf, "%x", va_arg(ap, int));
                str = string_append_cstr(str, buf);
                break;
            case 'f':
                /* ... will pass float as double */
                sprintf(buf, "%lf", va_arg(ap, double));
                str = string_append_cstr(str, buf);
                break;
                /* ... will pass char  as int */
            case 'c':
                str = string_append_char(str, va_arg(ap, int));
                break;
            case 's': {
                str = string_append_cstr(str, va_arg(ap, char*));
                break;
            }
            case 'P':
            case 'p': {
                sprintf(buf, "%p", va_arg(ap, void*));
                str = string_append_cstr(str, buf);
                break;
            }
            case 'o': {
                int next = fmt[i+1];
                MpObj v  = va_arg(ap, MpObj);
                if (next == 's') {
                    // "%os": 对象的字符串(没有引号)
                    str = string_append_obj(str, v);
                    i++;
                } else if (next == 't') {
                    // "%ot": 对象的类型
                    str = string_append_cstr(str, get_object_type_cstr(v));
                    i++;
                } else {
                    // "%o": 对象格式化
                    if (IS_STR(v)) {
                        str = string_append_char(str, '"');
                        str = string_append_obj(str, v);
                        str = string_append_char(str, '"');
                    } else {
                        str = string_append_obj(str, v);
                    }
                }
                break;
            }
            default:
                mp_raise("mp_format_va_list(%d): unknown pattern '%c' at idx=%d", 
                    __LINE__, fmt[i], i);
                break;
            }
        } else {
            str = string_append_char(str, fmt[i]);
        }
    }
    if (append_newline) {
        str = string_append_char(str, '\n');
    }
    return str;
}

MpObj mp_format(char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    MpObj v = mp_format_va_list(fmt, a, 0);
    va_end(a);
    return v;
}

MpObj mp_format_check_length(char* fmt, int ap_length, ...) {
    va_list ap;
    va_start(ap, ap_length);
    MpObj v = mp_format_va_list_check_length(fmt, ap, ap_length, 0);
    va_end(ap);
    return v;
}

void mp_printf(char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    mp_print(mp_format_va_list(fmt, a, 0));
    va_end(a);
}

MpObj bf_input() {
    int i = 0;
    if (mp_has_next_arg()) {
        mp_print(mp_take_obj_arg("input"));
    }
    char buf[2048];
    memset(buf, '\0', sizeof(buf));
    char* result = fgets(buf, sizeof(buf), stdin);
    if (result == NULL) {
        mp_raise("input: fgets failed");
    }
    int len = strlen(buf);
    /* if last char is '\n', we shift it, mainly in tcc */
    if(buf[len-1]=='\n'){
        buf[len-1] = '\0';
    }
    return string_new(buf);
}

MpObj bf_int() {
    MpObj v = mp_take_obj_arg("int");
    if (v.type == TYPE_NUM) {
        return number_obj((int) GET_NUM(v));
    } else if (v.type == TYPE_STR) {
        return number_obj((int) atof(GET_CSTR(v)));
    }
    mp_raise("int: %o can not be parsed to int ", v);
    return NONE_OBJECT;
}

MpObj bf_float() {
    MpObj v = mp_take_obj_arg("float");
    if (v.type == TYPE_NUM) {
        return v;
    } else if (v.type == TYPE_STR) {
        return number_obj(atof(GET_CSTR(v)));
    }
    mp_raise("float: %o can not be parsed to float", v);
    return NONE_OBJECT;
}

/**
 *   load_module( filename, code, mod_name = None )
 */
MpObj bf_load_module() {
    const char* sz_fnc = "load_module";
    MpObj file = mp_take_str_obj_arg(sz_fnc);
    MpObj code = mp_take_str_obj_arg(sz_fnc);
    MpObj name = NONE_OBJECT;
    if (mp_count_arg() == 3) {
        name = mp_take_str_obj_arg(sz_fnc);
    }
    return load_file_module(file, code, name);
}


/* get globals */
MpObj bf_globals() {
    return obj_get_globals(tm->frame->fnc);
}

/* get object type */

MpObj bf_exit() {
    longjmp(tm->frames->buf, 2);
    return NONE_OBJECT;
}

MpObj bf_gettype() {
    MpObj obj = mp_take_obj_arg("gettype");
    switch(MP_TYPE(obj)) {
        case TYPE_STR: return string_from_cstr("string");
        case TYPE_NUM: return string_from_cstr("number");
        case TYPE_LIST: return string_from_cstr("list");
        case TYPE_DICT: return string_from_cstr("dict");
        case TYPE_FUNCTION: return string_from_cstr("function");
        case TYPE_CLASS: return string_from_cstr("class");
        case TYPE_DATA: return string_from_cstr("data");
        case TYPE_NONE: return string_from_cstr("None");
        default: mp_raise("gettype(%o)", obj);
    }
    return NONE_OBJECT;
}

/**
 * bool istype(obj, str_type);
 * this function is better than `gettype(obj) == 'string'`;
 * think that you want to check a `basetype` which contains both `number` and `string`;
 * so, a check function with less result is better.
 */
MpObj bf_istype() {
    MpObj obj = mp_take_obj_arg("istype");
    char* type = mp_take_cstr_arg("istype");
    int is_type = 0;
    switch(MP_TYPE(obj)) {
        case TYPE_STR: is_type = strcmp(type, "string") == 0 ; break;
        case TYPE_NUM: is_type = strcmp(type, "number") == 0 ; break;
        case TYPE_LIST: is_type = strcmp(type, "list") == 0; break;
        case TYPE_DICT: is_type = strcmp(type, "dict") == 0; break;
        case TYPE_FUNCTION: is_type = strcmp(type, "function") == 0;break;
        case TYPE_DATA: is_type = strcmp(type, "data") == 0; break;
        case TYPE_CLASS: is_type = strcmp(type, "class") == 0; break;
        case TYPE_NONE: is_type = strcmp(type, "None") == 0; break;
        default: mp_raise("gettype(%o)", obj);
    }
    return number_obj(is_type);
}

MpObj bf_chr() {
    int n = mp_take_int_arg("chr");
    return string_chr(n);
}

MpObj bf_ord() {
    MpObj c = mp_take_str_obj_arg("ord");
    MP_ASSERT(GET_STR_LEN(c) == 1, "ord() expected a character");
    return number_obj((unsigned char) GET_CSTR(c)[0]);
}

MpObj bf_code8() {
    int n = mp_take_int_arg("code8");
    if (n < 0 || n > 255)
        mp_raise("code8(): expect number 0-255, but see %d", n);
    return string_chr(n);
}

MpObj bf_code16() {
    int n = mp_take_int_arg("code16");
    if (n < 0 || n > 0xffff)
        mp_raise("code16(): expect number 0-0xffff, but see %x", n);
    MpObj nchar = string_alloc(NULL, 2);
    code16((unsigned char*) GET_CSTR(nchar), n);
    return nchar;
}

MpObj bf_code32() {
    int n = mp_take_int_arg("code32");
    MpObj c = string_alloc(NULL, 4);
    code32((unsigned char*) GET_CSTR(c), n);
    return c;
}

MpObj bf_raise() {
    if (mp_count_arg() == 0) {
        mp_raise("raise");
    } else {
        mp_raise("%s", mp_take_cstr_arg("raise"));
    }
    return NONE_OBJECT;
}

MpObj bf_system() {
    MpObj m = mp_take_str_obj_arg("system");
    int rs = system(GET_CSTR(m));
    return number_obj(rs);
}

MpObj bf_str() {
    MpObj a = mp_take_obj_arg("str");
    return obj_str(a);
}

MpObj bf_list() {
    int args = mp_count_arg();
    if (args != 1) {
        mp_raise("list expected at most 1 arguments, got %d", args);
    }

    MpObj iterable = mp_take_obj_arg("list");
    MpObj iter   = iter_new(iterable);
    MpObj result = list_new(10);

    MpObj* next = obj_next(iter);
    while (next != NULL) {
        obj_append(result, *next);
        next = obj_next(iter);
    }
    return result;
}

MpObj bf_bool() {
    MpObj a = mp_take_obj_arg("bool");
    if (mp_is_true(a)) {
        return tm->_TRUE;
    } else {
        return tm->_FALSE;
    }
}

MpObj bf_dict() {
    return dict_new();
}

MpObj bf_len() {
    MpObj o = mp_take_obj_arg("len");
    return number_obj(mp_len(o));
}

MpObj bf_print() {
    int i = 0;
    while (mp_has_next_arg()) {
        mp_print(mp_take_obj_arg("print"));
        if (mp_has_next_arg()) {
            putchar(' ');
        }
    }
    putchar('\n');
    return NONE_OBJECT;
}

MpObj bf_file_append() {
    char* fname = mp_take_cstr_arg("file_append");
    MpObj content = mp_take_str_obj_arg("file_append");
    FILE* fp = fopen(fname, "ab+");
    if (fp == NULL) {
        mp_raise("file_append: fail to open file %s", fname);
        return NONE_OBJECT;
    }
    char* txt = GET_CSTR(content);
    int len = GET_STR_LEN(content);
    fwrite(txt, 1, len, fp);
    fclose(fp);
    return NONE_OBJECT;
}

//============
// remove file
//============
MpObj bf_remove(){
    MpObj fname = mp_take_str_obj_arg("remove");
    int flag = remove(GET_CSTR(fname));
    if(flag) {
        return number_obj(0);
    } else {
        return number_obj(1);
    }
}

MpObj bf_apply() {
    MpObj func = mp_take_obj_arg("apply");
    if (NOT_FUNC(func) && NOT_DICT(func) && NOT_CLASS(func)) {
        mp_raise("apply: expect function or dict");
    }
    MpObj args = mp_take_obj_arg("apply");
    mp_reset_args();
    if (IS_NONE(args)) {
    } else if(IS_LIST(args)) {
        int i;for(i = 0; i < LIST_LEN(args); i++) {
            mp_push_arg(LIST_NODES(args)[i]);
        }
    } else {
        mp_raise("apply: expect list arguments or None, but see %o", args);
        return NONE_OBJECT;
    }
    return MP_CALL_EX(func);
}

MpObj bf_write() {
    MpObj fmt = mp_take_obj_arg("write");
    MpObj str = obj_str(fmt);
    char* s = GET_CSTR(str);
    int len = GET_STR_LEN(str);
    int i;
    // for(i = 0; i < len; i++) {
        // mp_putchar(s[i]);
        // putchar is very very slow
        // when print 80 * 30 chars  
        //     ==>  putchar_time=126, printf_time=2, putc_time=129
        // putchar(s[i]);
        // buffer[i] = s[i];
    // }
    printf("%s", s);
    // return list_from_array(2, number_obj(t2-t1), number_obj(t3-t2));
    return NONE_OBJECT;
}

MpObj bf_pow() {
    double base = mp_take_double_arg("pow");
    double y = mp_take_double_arg("pow");
    return number_obj(pow(base, y));
}


MpObj* range_next(MpData* data) {
    long cur = data->cur;
    if (data->inc > 0 && cur < data->end) {
        data->cur += data->inc;
        data->cur_obj = number_obj(cur);
        return &data->cur_obj;
    } else if (data->inc < 0 && cur > data->end) {
        data->cur += data->inc;
        data->cur_obj = number_obj(cur);
        return &data->cur_obj;
    }
    return NULL;
}

MpObj bf_xrange() {
    long start = 0;
    long end = 0;
    int inc;
    static const char* sz_func = "range";
    switch (tm->arg_cnt) {
    case 1:
        start = 0;
        end = (long)mp_take_double_arg(sz_func);
        inc = 1;
        break;
    case 2:
        start = (long)mp_take_double_arg(sz_func);
        end = (long)mp_take_double_arg(sz_func);
        inc = 1;
        break;
    case 3:
        start = (long)mp_take_double_arg(sz_func);
        end   = (long)mp_take_double_arg(sz_func);
        inc   = (long)mp_take_double_arg(sz_func);
        break;
    default:
        mp_raise("range([n, [ n, [n]]]), but see %d arguments",
                tm->arg_cnt);
    }
    if (inc == 0)
        mp_raise("range(): increment can not be 0!");
    if (inc * (end - start) < 0)
        mp_raise("range(%d, %d, %d): not valid range!", start, end, inc);
    MpObj data = data_new(0);
    MpData *iterator = GET_DATA(data);
    iterator->cur  = start;
    iterator->end  = end;
    iterator->inc  = inc;
    iterator->next = range_next;

    return data;
}

MpObj bf_range() {
    return bf_xrange();
}

MpObj* enumerate_next(MpData* iterator) {
    MpObj iter = iterator->data_ptr[0];
    MpObj* next_value = obj_next(iter);

    if (next_value == NULL) {
        return NULL;
    } else {
        int idx = iterator->cur;
        iterator->cur += 1;
        iterator->cur_obj = list_from_array(2, number_obj(idx), *next_value);
        return &iterator->cur_obj;
    }
}

MpObj bf_enumerate() {
    MpObj _it = mp_take_obj_arg("enumerate");
    MpObj origin_iter = iter_new(_it);

    MpObj data = data_new(1);
    MpData* iterator = GET_DATA(data);
    iterator->cur = 0;
    iterator->data_ptr[0] = origin_iter;
    iterator->next = enumerate_next;
    return data;
}

MpObj bf_mmatch() {
    char* str = mp_take_cstr_arg("mmatch");
    int start = mp_take_int_arg("mmatch");
    MpObj o_dst = mp_take_str_obj_arg("mmatch");
    char* dst = GET_CSTR(o_dst);
    int size = GET_STR_LEN(o_dst);
    return number_obj(strncmp(str+start, dst, size) == 0);
}

long mp_clock() {
#ifdef _WIN32
    return clock();
#else
    return (double)clock()/1000;
#endif
}

MpObj bf_add_obj_method() {
    static const char* sz_func = "add_obj_method";
    MpObj type = mp_take_str_obj_arg(sz_func);
    MpObj fname = mp_take_str_obj_arg(sz_func);
    MpObj fnc = mp_take_func_obj_arg(sz_func);
    char*s = GET_CSTR(type);
    if (strcmp(s, "str") == 0) {
        obj_set(tm->str_proto, fname, fnc);
    } else if (strcmp(s, "list") == 0) {
        obj_set(tm->list_proto, fname, fnc);
    } else if (strcmp(s, "dict") == 0) {
        obj_set(tm->dict_proto, fname, fnc);
    } else {
        mp_raise("add_obj_method: not valid object type, expect str, list, dict");
    }
    return NONE_OBJECT;
}

MpObj bf_read_file() {
    static const char* sz_func = "read_file";
    char c;
    char* fname = mp_take_cstr_arg(sz_func);
    int nsize = mp_take_int_arg(sz_func);
    char buf[1024];
    int i;
    int end = 0;
    MpObj func;
    if (nsize < 0 || nsize > 1024) {
        mp_raise("%s: can not set bufsize beyond [1, 1024]",  sz_func);
    }
    func = mp_take_func_obj_arg(sz_func);
    FILE* fp = fopen(fname, "rb");
    if (fp == NULL) {
        mp_raise("%s: can not open file %s", sz_func, fname);
    }
    while (1) {
        mp_reset_args();
        for (i = 0; i < nsize; i++) {
            if ((c = fgetc(fp)) != EOF) {
                buf[i] = c;
            } else {
                end = 1;
                break;
            }
        }
        mp_push_arg(string_alloc(buf, i));
        MP_CALL_EX(func);
        if (end) {
            break;
        }
    }
    fclose(fp);
    return NONE_OBJECT;
}

MpObj bf_iter() {
    MpObj func = mp_take_obj_arg("iter");
    return iter_new(func);
}

MpObj bf_next() {
    MpObj iter = mp_take_data_obj_arg("next");
    MpObj *ret = obj_next(iter);
    if (ret == NULL) {
        mp_raise("<<next end>>");
        return NONE_OBJECT;
    } else {
        return *ret;
    }
}

MpObj bf_get_os_name() {
    const char* sz_func = "getosname";
#ifdef _WINDOWS_H
    return string_from_cstr("nt");
#else
    return string_from_cstr("posix");
#endif
}

MpObj bf_mp_traceback() {
    mp_traceback();
    return NONE_OBJECT;
}

/**
 * create a object in tm
 */ 
MpObj bf_newobj() {
    MpObj obj = dict_new();
    return obj;
}

/**
 * random
 */
MpObj bf_random() {
    static long seed = 0;
    if (seed == 0) {
        seed = time(NULL);
        srand(seed);
    }
    int n = rand() % 77;
    // printf("%d\n", n);
    double val = (double)((double) n / (double)77);
    return number_obj(val);
}

MpObj bf_Exception() {
    return mp_take_obj_arg("Exception");
}

MpObj bf_getattr() {
    MpObj self = mp_take_obj_arg("getattr");
    MpObj key  = mp_take_obj_arg("getattr");
    return obj_get(self, key);
}

MpObj bf_setattr() {
    MpObj self = mp_take_obj_arg("setattr");
    MpObj key  = mp_take_obj_arg("setattr");
    MpObj val  = mp_take_obj_arg("setattr");
    obj_set(self, key, val);
    return NONE_OBJECT;
}

MpObj bf_hasattr() {
    MpObj self = mp_take_obj_arg("hasattr");
    MpObj key  = mp_take_obj_arg("hasattr");
    return obj_is_in(self, key);
}

MpObj bf_hash() {
    MpObj object = mp_take_obj_arg("hash");
    return number_obj(mp_get_obj_hash(object));
}

MpObj bf_isinstance() {
    MpObj first = mp_take_obj_arg("isinstance");
    MpObj type = mp_take_obj_arg("isinstance");
    if (IS_FUNC(type) && GET_FUNCTION(type)->native == bf_list && IS_LIST(first)) {
        return tm->_TRUE;
    }
    mp_raise("bf_isinstance: unsupported check");
    return tm->_FALSE;
}

void mp_init_builtins() {
    mp_reg_builtin_func("file_append", bf_file_append);
    mp_reg_builtin_func("remove", bf_remove);
    mp_reg_builtin_func("write", bf_write);
    mp_reg_builtin_func("load_module", bf_load_module);
    mp_reg_builtin_func("gettype", bf_gettype);
    mp_reg_builtin_func("istype", bf_istype);
    mp_reg_builtin_func("code8", bf_code8);
    mp_reg_builtin_func("code16", bf_code16);
    mp_reg_builtin_func("code32", bf_code32);
    mp_reg_builtin_func("mmatch", bf_mmatch);
    mp_reg_builtin_func("newobj", bf_newobj);

    /* python built-in functions */
    mp_reg_builtin_func("globals", bf_globals);
    mp_reg_builtin_func("len", bf_len);
    mp_reg_builtin_func("exit", bf_exit);
    mp_reg_builtin_func("input", bf_input);

    /* builtin type */
    mp_reg_builtin_func("str", bf_str);
    mp_reg_builtin_func("int", bf_int);
    mp_reg_builtin_func("float", bf_float);
    mp_reg_builtin_func("bool", bf_bool);
    mp_reg_builtin_func("list", bf_list);
    mp_reg_builtin_func("dict", bf_dict);

    mp_reg_builtin_func("print", bf_print);
    mp_reg_builtin_func("chr", bf_chr);
    mp_reg_builtin_func("ord", bf_ord);
    mp_reg_builtin_func("raise", bf_raise);
    mp_reg_builtin_func("system", bf_system);
    mp_reg_builtin_func("apply", bf_apply);
    mp_reg_builtin_func("pow", bf_pow);
    mp_reg_builtin_func("range",  bf_range);
    mp_reg_builtin_func("xrange", bf_xrange);
    mp_reg_builtin_func("enumerate", bf_enumerate);
    mp_reg_builtin_func("random", bf_random);
    mp_reg_builtin_func("Exception", bf_Exception);
    mp_reg_builtin_func("getattr", bf_getattr);
    mp_reg_builtin_func("setattr", bf_setattr);
    mp_reg_builtin_func("hasattr", bf_hasattr);
    
    /* functions which has impact on vm follow camel case */
    // mp_reg_builtin_func("get_const_idx", bf_get_const_idx);
    // mp_reg_builtin_func("get_const", bf_get_const);
    // mp_reg_builtin_func("get_const_len", bf_get_const_len);
    mp_reg_builtin_func("traceback", bf_mp_traceback);

    mp_reg_builtin_func("add_obj_method", bf_add_obj_method);
    mp_reg_builtin_func("read_file", bf_read_file);
    mp_reg_builtin_func("iter", bf_iter);
    mp_reg_builtin_func("next", bf_next);
    
    mp_reg_builtin_func("getosname", bf_get_os_name);
    mp_reg_builtin_func("hash", bf_hash);
    mp_reg_builtin_func("isinstance", bf_isinstance);
}


