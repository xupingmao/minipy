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
void mp_reset_args() {
    tm->arguments = tm->internal_arg_stack;
    tm->arg_loaded = 0;
    tm->arg_cnt = 0;
}

void mp_push_arg(MpObj obj) {
    tm->arguments = tm->internal_arg_stack;
    tm->arg_loaded = 0;
    tm->arguments[tm->arg_cnt] = obj;
    tm->arg_cnt += 1;
    if(tm->arg_cnt > MAX_ARG_COUNT) {
        mp_raise("mp_push_arg(): too many arguments! over %d", MAX_ARG_COUNT);
    }
}

void mp_set_args(MpObj* first, int len) {
    tm->arguments = first;
    tm->arg_cnt = len;
    tm->arg_loaded = 0;
}

void mp_insert_arg(MpObj first) {
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

void mp_resolve_self_by_func_ptr(MpFunction *fnc) {
    if(IS_NONE(fnc->self)) {
        return;
    }
    mp_insert_arg(fnc->self);
}

MpObj mp_take_arg_from_vm0(const char* fnc) {
    if (tm->arg_loaded >= tm->arg_cnt)
        mp_raise("%s :no argument! total %d, current %d",
                fnc, tm->arg_cnt, tm->arg_loaded);
    tm->arg_loaded += 1;
    return tm->arguments[tm->arg_loaded - 1];
}


int mp_has_next_arg() {
    return tm->arg_loaded < tm->arg_cnt;
}

int mp_count_remain_args () {
    return tm->arg_cnt - tm->arg_loaded;
}

MpObj mp_take_str_obj_arg(const char* fnc) {
    MpObj value = mp_take_arg_from_vm0(fnc);
    if (NOT_STR(value)) {
        mp_raise("%s: expect string but see %s", fnc, get_type_cstr(value.type));
    }
    return value;
}

MpStr* mp_take_str_ptr_arg(const char* fnc) {
    MpObj value = mp_take_arg_from_vm0(fnc);
    if (NOT_STR(value)) {
        mp_raise("%s: expect string but see %s", fnc, get_type_cstr(value.type));
    }
    return value.value.str;
}

char* mp_take_cstr_arg(const char* fnc) {
    MpObj value = mp_take_arg_from_vm0(fnc);
    if (NOT_STR(value)) {
        mp_raise("%s: expect string but see %s", fnc, get_type_cstr(value.type));
    }
    return GET_CSTR(value);
}

MpObj mp_take_func_obj_arg(const char* fnc) {
    MpObj value = mp_take_arg_from_vm0(fnc);
    if (NOT_FUNC(value)) {
        mp_raise("%s: expect function but see %s", fnc, get_type_cstr(value.type));
    }
    return value;
}

MpObj mp_take_list_obj_arg(const char* fnc) {
    MpObj v = mp_take_arg_from_vm0(fnc);
    if (NOT_LIST(v)) {
        mp_raise("%s: expect list but see %s", fnc, get_type_cstr(v.type));
    }
    return v;
}

MpObj mp_take_dict_obj_arg(const char* fnc) {
    MpObj v = mp_take_arg_from_vm0(fnc);
    if (NOT_DICT(v)) {
        mp_raise("%s: expect dict but see %s", fnc, get_type_cstr(v.type));
    }
    return v;
}

MpDict* mp_take_dict_ptr_arg(const char* fnc) {
    MpObj v = mp_take_dict_obj_arg(fnc);
    return GET_DICT(v);
}

MpList* mp_take_list_ptr_arg(const char* fnc) {
    MpObj v = mp_take_arg_from_vm0(fnc);
    if (NOT_LIST(v)) {
        mp_raise("%s: expect list but see %s", fnc, get_type_cstr(v.type));
    }
    return GET_LIST(v);
}

int mp_take_int_arg(const char* fnc) {
    MpObj v = mp_take_arg_from_vm0(fnc);
    if (MP_TYPE(v) != TYPE_NUM) {
        mp_raise("%s: expect number but see %s", fnc, get_type_cstr(v.type));
    }
    return (int) GET_NUM(v);
}

double mp_take_double_arg(const char* fnc) {
    MpObj v = mp_take_arg_from_vm0(fnc);
    if (MP_TYPE(v) != TYPE_NUM) {
        mp_raise("%s: expect number but see %s", fnc, get_type_cstr(v.type));
    }
    return GET_NUM(v);
}

MpObj mp_take_obj_arg(const char* fnc) {
    return mp_take_arg_from_vm0(fnc);
}

MpObj mp_take_data_obj_arg(const char* fnc) {
    MpObj v = mp_take_arg_from_vm0(fnc);
    if (MP_TYPE(v) != TYPE_DATA) {
        mp_raise("%s: expect data but see %s", fnc, get_type_cstr(v.type));
    }
    return v;
}

int mp_count_arg() {
    return tm->arg_cnt;
}
