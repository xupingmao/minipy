/**
 * minipy arguments module
 * @author xupingmao
 * @since 2016
 * @modified 2022/01/12 22:17:11
 */

#include "include/mp.h"

/**
 * reset argument list
 * call this function before set argument
 */
void arg_start() {
    tm->arguments = tm->internal_arg_stack;
    tm->arg_loaded = 0;
    tm->arg_cnt = 0;
}

void arg_push(MpObj obj) {
    tm->arguments = tm->internal_arg_stack;
    tm->arg_loaded = 0;
    tm->arguments[tm->arg_cnt] = obj;
    tm->arg_cnt += 1;
    if(tm->arg_cnt > MAX_ARG_COUNT) {
        mp_raise("arg_push(): too many arguments! over %d", MAX_ARG_COUNT);
    }
}

void arg_set_arguments(MpObj* first, int len) {
    tm->arguments = first;
    tm->arg_cnt = len;
    tm->arg_loaded = 0;
}

void arg_insert(MpObj first) {
    int i;
    for (i = tm->arg_cnt; i > 0; i--) {
        tm->arguments[i] = tm->arguments[i-1];
    }
    #if 0
        mp_printf("Self = %o\n", first);
    #endif
    tm->arguments[0] = first;
    tm->arg_cnt += 1;
}

void resolve_self_by_func_ptr(MpFunction *fnc) {
    if(IS_NONE(fnc->self)) {
        return;
    }
    arg_insert(fnc->self);
}

MpObj arg_take_from_vm0(const char* fnc) {
    if (tm->arg_loaded >= tm->arg_cnt)
        mp_raise("%s :no argument! total %d, current %d",
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

MpObj arg_take_str_obj(const char* fnc) {
    MpObj value = arg_take_from_vm0(fnc);
    if (NOT_STR(value)) {
        mp_raise("%s: expect string but see %s", fnc, get_type_cstr(value.type));
    }
    return value;
}

MpStr* arg_take_str_ptr(const char* fnc) {
    MpObj value = arg_take_from_vm0(fnc);
    if (NOT_STR(value)) {
        mp_raise("%s: expect string but see %s", fnc, get_type_cstr(value.type));
    }
    return value.value.str;
}

char* arg_take_cstr(const char* fnc) {
    MpObj value = arg_take_from_vm0(fnc);
    if (NOT_STR(value)) {
        mp_raise("%s: expect string but see %s", fnc, get_type_cstr(value.type));
    }
    return GET_CSTR(value);
}

MpObj arg_take_func_obj(const char* fnc) {
    MpObj value = arg_take_from_vm0(fnc);
    if (NOT_FUNC(value)) {
        mp_raise("%s: expect function but see %s", fnc, get_type_cstr(value.type));
    }
    return value;
}

MpObj arg_take_list_obj(const char* fnc) {
    MpObj v = arg_take_from_vm0(fnc);
    if (NOT_LIST(v)) {
        mp_raise("%s: expect list but see %s", fnc, get_type_cstr(v.type));
    }
    return v;
}

MpObj arg_take_dict_obj(const char* fnc) {
    MpObj v = arg_take_from_vm0(fnc);
    if (NOT_DICT(v)) {
        mp_raise("%s: expect dict but see %s", fnc, get_type_cstr(v.type));
    }
    return v;
}

MpList* arg_take_list_ptr(const char* fnc) {
    MpObj v = arg_take_from_vm0(fnc);
    if (NOT_LIST(v)) {
        mp_raise("%s: expect list but see %s", fnc, get_type_cstr(v.type));
    }
    return GET_LIST(v);
}

int arg_take_int(const char* fnc) {
    MpObj v = arg_take_from_vm0(fnc);
    if (MP_TYPE(v) != TYPE_NUM) {
        mp_raise("%s: expect number but see %s", fnc, get_type_cstr(v.type));
    }
    return (int) GET_NUM(v);
}

double arg_take_double(const char* fnc) {
    MpObj v = arg_take_from_vm0(fnc);
    if (MP_TYPE(v) != TYPE_NUM) {
        mp_raise("%s: expect number but see %s", fnc, get_type_cstr(v.type));
    }
    return GET_NUM(v);
}

MpObj arg_take_obj(const char* fnc) {
    return arg_take_from_vm0(fnc);
}

MpObj arg_take_data_obj(const char* fnc) {
    MpObj v = arg_take_from_vm0(fnc);
    if (MP_TYPE(v) != TYPE_DATA) {
        mp_raise("%s: expect data but see %s", fnc, get_type_cstr(v.type));
    }
    return v;
}

int arg_count() {
    return tm->arg_cnt;
}
