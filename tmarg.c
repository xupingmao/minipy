#include "include/tmarg.h"
#include "include/function.h"

void arg_start() {
    tm->arguments = tm->internalArgStack;
    tm->arg_loaded = 0;
    tm->arg_cnt = 0;
}

void arg_push(Object obj) {
    tm->arguments = tm->internalArgStack;
    tm->arg_loaded = 0;
    tm->arguments[tm->arg_cnt] = obj;
    tm->arg_cnt += 1;
    if(tm->arg_cnt > MAX_ARG_COUNT) {
        tm_raise("arg_push(): too many arguments! over %d", MAX_ARG_COUNT);
    }
}

void tm_setArguments(Object* first, int len) {
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

void _resolveMethodSelf(TmFunction *fnc) {
    if(IS_NONE(fnc->self)) {
        return;
    }
    arg_insert(fnc->self);
}

Object getArgFromVM0(const char* fnc) {
    if (tm->arg_loaded >= tm->arg_cnt)
        tm_raise("%s :no argument! total %d, current %d",
                fnc, tm->arg_cnt, tm->arg_loaded);
    tm->arg_loaded += 1;
    return tm->arguments[tm->arg_loaded - 1];
}


int hasArg() {
    return tm->arg_loaded < tm->arg_cnt;
}

Object getArgFromVM2(int type, const char* fnc) {
    Object value = getArgFromVM0(fnc);
    if (type == -1)
        return value;
    if (type != value.type)
        tm_raise("%s:expect type %s, but see %s", fnc, getTypeByInt(type),
                getTypeByObj(value));
    return value;
}


Object arg_get_str(const char* fnc) {
    Object value = getArgFromVM0(fnc);
    if (NOT_STR(value)) {
        tm_raise("%s: expect string but see %s", fnc, getTypeByObj(value));
    }
    return value;
}

char* arg_get_sz(const char* fnc) {
    Object value = getArgFromVM0(fnc);
    if (NOT_STR(value)) {
        tm_raise("%s: expect string but see %s", fnc, getTypeByObj(value));
    }
    return GET_STR(value);
}

Object arg_get_func(const char* fnc) {
    Object value = getArgFromVM0(fnc);
    if (NOT_FUNC(value)) {
        tm_raise("%s: expect function but see %s", fnc, getTypeByObj(value));
    }
    return value;
}

Object arg_get_list(const char* fnc) {
    Object v = getArgFromVM0(fnc);
    if (NOT_LIST(v)) {
        tm_raise("%s: expect list but see %s", fnc, getTypeByObj(v));
    }
    return v;
}

Object arg_get_dict(const char* fnc) {
    Object v = getArgFromVM0(fnc);
    if (NOT_DICT(v)) {
        tm_raise("%s: expect dict but see %s", fnc, getTypeByObj(v));
    }
    return v;
}

TmList* arg_get_list_p(const char* fnc) {
    Object v = getArgFromVM0(fnc);
    if (NOT_LIST(v)) {
        tm_raise("%s: expect list but see %s", fnc, getTypeByObj(v));
    }
    return GET_LIST(v);
}

int arg_get_int(const char* fnc) {
    Object v = getArgFromVM0(fnc);
    if (TM_TYPE(v) != TYPE_NUM) {
        tm_raise("%s: expect number but see %s", fnc, getTypeByObj(v));
    }
    return (int) GET_NUM(v);
}

double arg_get_double(const char* fnc) {
    Object v = getArgFromVM0(fnc);
    if (TM_TYPE(v) != TYPE_NUM) {
        tm_raise("%s: expect number but see %s", fnc, getTypeByObj(v));
    }
    return GET_NUM(v);
}

Object arg_get_obj(const char* fnc) {
    return getArgFromVM0(fnc);
}

Object arg_get_data(const char* fnc) {
    Object v = getArgFromVM0(fnc);
    if (TM_TYPE(v) != TYPE_DATA) {
        tm_raise("%s: expect data but see %s", fnc, getTypeByObj(v));
    }
    return v;
}
int getArgsCount() {
    return tm->arg_cnt;
}
