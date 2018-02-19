/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2018/02/19 16:44:18
 */

#include "include/tm.h"
// 
// argument functions
// 
/**
 * reset argument list
 * call this function before set argument
 */
void arg_start() {
    tm->arguments = tm->internal_arg_stack;
    tm->arg_loaded = 0;
    tm->arg_cnt = 0;
}

void arg_push(Object obj) {
    tm->arguments = tm->internal_arg_stack;
    tm->arg_loaded = 0;
    tm->arguments[tm->arg_cnt] = obj;
    tm->arg_cnt += 1;
    if(tm->arg_cnt > MAX_ARG_COUNT) {
        tm_raise("arg_push(): too many arguments! over %d", MAX_ARG_COUNT);
    }
}

void arg_set_arguments(Object* first, int len) {
    tm->arguments = first;
    tm->arg_cnt = len;
    tm->arg_loaded = 0;
}

void arg_insert(Object first) {
    int i;
    for (i = tm->arg_cnt; i > 0; i--) {
        tm->arguments[i] = tm->arguments[i-1];
    }
    #if 0
        tm_printf("Self = %o\n", first);
    #endif
    tm->arguments[0] = first;
    tm->arg_cnt += 1;
}

void _resolve_method_self(TmFunction *fnc) {
    if(IS_NONE(fnc->self)) {
        return;
    }
    arg_insert(fnc->self);
}

Object arg_take_obj(const char* fnc) {
    if (tm->arg_loaded >= tm->arg_cnt)
        tm_raise("%s :no argument! total %d, current %d",
                fnc, tm->arg_cnt, tm->arg_loaded);
    tm->arg_loaded += 1;
    return tm->arguments[tm->arg_loaded - 1];
}


int arg_has_next() {
    return tm->arg_loaded < tm->arg_cnt;
}

int arg_remains () {
    return tm->arg_cnt - tm->arg_loaded;
}

Object arg_take_str_obj(const char* fnc) {
    Object value = arg_take_obj(fnc);
    if (NOT_STR(value)) {
        tm_raise("%s: expect string but see %s", fnc, tm_type(value.type));
    }
    return value;
}

String* arg_take_str_ptr(const char* fnc) {
    Object value = arg_take_obj(fnc);
    if (NOT_STR(value)) {
        tm_raise("%s: expect string but see %s", fnc, tm_type(value.type));
    }
    return value.value.str;
}

char* arg_take_sz(const char* fnc) {
    Object value = arg_take_obj(fnc);
    if (NOT_STR(value)) {
        tm_raise("%s: expect string but see %s", fnc, tm_type(value.type));
    }
    return GET_STR(value);
}

Object arg_take_func_obj(const char* fnc) {
    Object value = arg_take_obj(fnc);
    if (NOT_FUNC(value)) {
        tm_raise("%s: expect function but see %s", fnc, tm_type(value.type));
    }
    return value;
}

Object arg_take_list_obj(const char* fnc) {
    Object v = arg_take_obj(fnc);
    if (NOT_LIST(v)) {
        tm_raise("%s: expect list but see %s", fnc, tm_type(v.type));
    }
    return v;
}

Object arg_take_dict_obj(const char* fnc) {
    Object v = arg_take_obj(fnc);
    if (NOT_DICT(v)) {
        tm_raise("%s: expect dict but see %s", fnc, tm_type(v.type));
    }
    return v;
}

TmList* arg_take_list_ptr(const char* fnc) {
    Object v = arg_take_obj(fnc);
    if (NOT_LIST(v)) {
        tm_raise("%s: expect list but see %s", fnc, tm_type(v.type));
    }
    return GET_LIST(v);
}

int arg_take_int(const char* fnc) {
    Object v = arg_take_obj(fnc);
    if (TM_TYPE(v) != TYPE_NUM) {
        tm_raise("%s: expect number but see %s", fnc, tm_type(v.type));
    }
    return (int) GET_NUM(v);
}

double arg_take_double(const char* fnc) {
    Object v = arg_take_obj(fnc);
    if (TM_TYPE(v) != TYPE_NUM) {
        tm_raise("%s: expect number but see %s", fnc, tm_type(v.type));
    }
    return GET_NUM(v);
}

Object arg_take_data_obj(const char* fnc) {
    Object v = arg_take_obj(fnc);
    if (TM_TYPE(v) != TYPE_DATA) {
        tm_raise("%s: expect data but see %s", fnc, tm_type(v.type));
    }
    return v;
}
int get_args_count() {
    return tm->arg_cnt;
}
