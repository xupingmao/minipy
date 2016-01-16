#include "include/tm.h"

#define RET_NUM(v) \
    Object _num;\
    TM_TYPE(_num) = TYPE_NUM;\
    GET_NUM(_num) = v;\
    return _num;

void tmAssertType(Object o, int type, char* msg) {
    if (TM_TYPE(o) != type) {
        tm_raise("%s, expect %s but see %s", msg, 
            getTypeByInt(type), getTypeByObj(o));
    }
}

void tmAssertInt(double value, char* msg) {
    if (value != (int) value) {
        tm_raise("%s require int", msg);
    }
}

int objEqSz(Object obj, const char* value) {
    return TM_TYPE(obj) == TYPE_STR
            && (GET_STR(obj) == value || strcmp(GET_STR(obj), value) == 0);
}

void tm_set(Object self, Object k, Object v) {
    switch (TM_TYPE(self)) {
    case TYPE_LIST: {
        tmAssertType(k, TYPE_NUM, "tm_set");
        double d = GET_NUM(k);
        tmAssertInt(d, "listSet");
        list_set(GET_LIST(self), (int)d, v);
    }
        return;
    case TYPE_DICT:
        dict_set(GET_DICT(self), k, v);
        return;
    }
    tm_raise("tm_set: Self %o, Key %o, Val %o", self, k, v);
}

Object tm_get(Object self, Object k) {
    Object v;
    switch (TM_TYPE(self)) {
    case TYPE_STR: {
        DictNode* node;
        if (TM_TYPE(k) == TYPE_NUM) {
            double d = GET_NUM(k);
            tmAssertInt(d, "stringGet");
            int n = d;
            if (n < 0) {
                n += GET_STR_LEN(self);
            }
            if (n >= GET_STR_LEN(self) || n < 0)
                tm_raise("StringGet: index overflow ,len=%d,index=%d, str=%o",
                        GET_STR_LEN(self), n, self);
            return string_chr(0xff & GET_STR(self)[n]);
        } else if ((node = dict_get_node(GET_DICT(tm->str_proto), k)) != NULL) {
            return method_new(node->val, self);
        }
        break;
    }
    case TYPE_LIST:{
        DictNode* node;
        if (TM_TYPE(k) == TYPE_NUM) {
            return list_get(GET_LIST(self), GET_NUM(k));
        } else if ((node = dict_get_node(GET_DICT(tm->list_proto), k))!=NULL) {
            return method_new(node->val, self);
        }
        break;
    }
    case TYPE_DICT:{
        DictNode* node;
        node = dict_get_node(GET_DICT(self), k);
        if (node != NULL) {
            return node->val;
        } else if ((node = dict_get_node(GET_DICT(tm->dict_proto), k))!=NULL) {
            return method_new(node->val, self);
        }
        break;
    }
    case TYPE_FUNCTION:
        return getFuncAttr(GET_FUNCTION(self), k);
        break;
    case TYPE_DATA:
        return GET_DATA_PROTO(self)->get(GET_DATA(self), k);
    }
    tm_raise("keyError %o", k);
    return NONE_OBJECT;
}

Object tm_sub(Object a, Object b) {
    if (a.type == b.type) {
        if (a.type == TYPE_NUM) {
            GET_NUM(a) -= GET_NUM(b);
            SET_IDX(a, 0);
            return a;
        }
    }
    tm_raise("tm_sub: can not substract %o and %o", a, b);
    return NONE_OBJECT;
}

Object tm_add(Object a, Object b) {
    if (TM_TYPE(a) == TM_TYPE(b)) {
        switch (TM_TYPE(a)) {
        case TYPE_NUM:
            GET_NUM(a) += GET_NUM(b);
            SET_IDX(a, 0);
            return a;
        case TYPE_STR: {
            char* sa = GET_STR(a);
            char* sb = GET_STR(b);
            int la = GET_STR_LEN(a);
            int lb = GET_STR_LEN(b);
            if (la == 0) {return b;    }
            if (lb == 0) {return a;    }
            int len = la + lb;
            Object des = string_alloc(NULL, len);
            char*s = GET_STR(des);
            memcpy(s, sa, la);
            memcpy(s + la, sb, lb);
            return des;
        }
        case TYPE_LIST: {
            return list_add(GET_LIST(a), GET_LIST(b));
        }
        }
    }
    tm_raise("tm_add: can not add %o and %o", (a), (b));
    return NONE_OBJECT;
}

int tm_equals(Object a, Object b){
    if(TM_TYPE(a) != TM_TYPE(b)) return 0;
    switch(TM_TYPE(a)){
        case TYPE_NUM:return GET_NUM(a) == GET_NUM(b);
        case TYPE_STR: {
            String* s1 = GET_STR_OBJ(a);
            String* s2 = GET_STR_OBJ(b);
            return s1->value == s2->value || 
                (s1->len == s2->len && strncmp(s1->value, s2->value, s1->len) == 0);
        }
        case TYPE_LIST:    {
            if(GET_LIST(a) == GET_LIST(b)) return 1;
            int i;
            int len = GET_LIST(a)->len;
            Object* nodes1 = GET_LIST(a)->nodes;
            Object* nodes2 = GET_LIST(b)->nodes;
            for(i = 0; i < len; i++){
                if(!tm_equals(nodes1[i], nodes2[i]) ){
                    return 0;
                }
            }
            return 1;
        }
        case TYPE_NONE:return 1;
        case TYPE_DICT:return GET_DICT(a) == GET_DICT(b);
        case TYPE_FUNCTION: return GET_FUNCTION(a) == GET_FUNCTION(b);
        default: tm_raise("equals(): not supported type %d", a.type);
    }
    return 0;
}

/*
#define DEF_CMP_FUNC(fnc_name, op) Object fnc_name(Object a, Object b) {      \
    if(TM_TYPE(a) != TM_TYPE(b))                             \
        tm_raise(#fnc_name"(): can not compare [%o] and [%o]", (a), (b));                 \
    switch(a.type){                      \
        case TYPE_NUM: { RET_NUM(GET_NUM(a) op GET_NUM(b) );} \
        case TYPE_STR: { RET_NUM(strcmp(GET_STR(a), GET_STR(b)) op 0); } \
        default : tm_raise(#fnc_name"() not support yet"); \
    }                           \
    return NONE_OBJECT;\
}

#define DEF_CMP_FUNC_2(fnc_name, op) int fnc_name(Object a, Object b) {      \
    if(TM_TYPE(a) != TM_TYPE(b))                             \
        tm_raise(#fnc_name"(): can not compare [%o] and [%o]", (a), (b));                 \
    switch(a.type){                      \
        case TYPE_NUM: return  GET_NUM(a) op GET_NUM(b); \
        case TYPE_STR: return  strcmp(GET_STR(a) , GET_STR(b)) op 0; \
        default : tm_raise(#fnc_name"() not support yet"); \
    }                           \
    return 0;\
}

DEF_CMP_FUNC(tmLessThan, <);
DEF_CMP_FUNC(tmGreaterThan, >);
DEF_CMP_FUNC(tmLessEqual, <=);
DEF_CMP_FUNC(tmGreaterEqual, >=);
*/

int tm_cmp(Object a, Object b) {
    if (TM_TYPE(a) == TM_TYPE(b)) {
        switch (TM_TYPE(a)) {
            case TYPE_NUM: {
                double diff = GET_NUM(a) - GET_NUM(b);
                if (diff > 0.0) {
                    return 1;
                } else if (diff < 0.0) {
                    return -1;
                }
                return 0;
            }
            case TYPE_STR: return strcmp(GET_STR(a), GET_STR(b));
        }
    }
    tm_raise("tm_cmp: can not compare %o and %o", a, b);
    return 0;
}
/*
DEF_CMP_FUNC_2(tm_bool_lt, <);
DEF_CMP_FUNC_2(tm_bool_gt, >);
DEF_CMP_FUNC_2(tm_bool_lteq, <=);
DEF_CMP_FUNC_2(tm_bool_gteq, >=);
*/

Object tm_mul(Object a, Object b) {
    if (a.type == b.type && a.type == TYPE_NUM) {
        GET_NUM(a) *= GET_NUM(b);
        SET_IDX(a, 0);
        return a;
    }
    if (a.type == TYPE_NUM && b.type == TYPE_STR) {
        Object temp = a;
        a = b;
        b = temp;
    }
    if (a.type == TYPE_STR && b.type == TYPE_NUM) {
        int len = GET_STR_LEN(a);
        Object des;
        if (len == 0)
            return a;
        int times = (int) GET_NUM(b);
        if (times <= 0)
            return string_static("");
        if (times == 1)
            return a;
        des = string_alloc(NULL, len * times);
        char* s = GET_STR(des);
        int i;
        for (i = 0; i < times; i++) {
            strncpy(s, GET_STR(a), len);
            s += len;
        }
        return des;
    }
    tm_raise("tm_mul: can not multiply %o and %o", a, b);
    return NONE_OBJECT;
}

Object tm_div(Object a, Object b) {
    if (a.type == b.type && a.type == TYPE_NUM) {
        GET_NUM(a) /= GET_NUM(b);
        SET_IDX(a, 0);
        return a;
    }
    tm_raise("tm_div: can not divide %o and %o", a, b);
    return NONE_OBJECT;
}

Object tm_mod(Object a, Object b) {
    if (a.type == b.type && a.type == TYPE_NUM) {
        return tm_number((long) GET_NUM(a) % (long) GET_NUM(b));
    } else if (a.type == TYPE_STR) {
        Object *__mod__ = getBuiltin("__mod__");
        if (__mod__ == NULL) {
            tm_raise("__mod__ is not defined");
        } else {
            arg_start();
            arg_push(a);
            arg_push(b);
            return callFunction(*__mod__);
        }        
    }
    tm_raise("tm_mod: can not module %o and %o", a, b);
    return NONE_OBJECT;
}

/* a has b */
int tm_has(Object a, Object b) {
    switch (TM_TYPE(a)) {
    case TYPE_LIST: {
        return (list_index(GET_LIST(a), b) != -1);
    }
    case TYPE_STR: {
        if (TM_TYPE(b) != TYPE_STR)
            return 0;
        return string_index(GET_STR_OBJ(a), GET_STR_OBJ(b), 0) != -1;
    }
    case TYPE_DICT: {
        DictNode* node = dict_get_node(GET_DICT(a), b);
        if (node == NULL) {
            return 0;
        }
        return 1;
    }
    }
    return 0;
}
int tm_bool(Object v) {
    switch (TM_TYPE(v)) {
    case TYPE_NUM:
        return GET_NUM(v) != 0;
    case TYPE_NONE:
        return 0;
    case TYPE_STR:
        return GET_STR_LEN(v) > 0;
    case TYPE_LIST:
        return LIST_LEN(v) > 0;
    case TYPE_DICT:
        return DICT_LEN(v) > 0;
    case TYPE_FUNCTION:
    case TYPE_DATA:
        return 1;
    }
    return 0;
}

Object tm_neg(Object o) {
    if (o.type == TYPE_NUM) {
        GET_NUM(o) = -GET_NUM(o);
        return o;
    }
    tm_raise("tm_neg: can not handle %o", o);
    return NONE_OBJECT;
}


Object iter_new(Object collections) {
    switch(TM_TYPE(collections)) {
        case TYPE_LIST: return listIterNew(GET_LIST(collections));
        case TYPE_DICT: return dict_iter_new(GET_DICT(collections));
        case TYPE_STR: return stringIterNew(GET_STR_OBJ(collections));
        case TYPE_DATA: return collections;
        default: tm_raise("iter_new(): can not iterate %o", collections);
    }
    return NONE_OBJECT;
}

Object* tm_next(Object iterator) {
    return GET_DATA_PROTO(iterator)->next(GET_DATA(iterator));
}

void tm_del(Object self, Object k) {
    switch(TM_TYPE(self)) {
        case TYPE_DICT:{
            dict_del(GET_DICT(self), k);
            break;
        }
        case TYPE_LIST:{
            list_del(GET_LIST(self), k);
            break;
        }
        default:
            tm_raise("tm_del: not supported type %s", getTypeByObj(self));
    }
}

Object tm_append(Object a, Object item) {
    if (IS_LIST(a)) {
        list_append(GET_LIST(a), item);
    }
    return a;
}

TmFrame* tm_getframe(int fidx) {
    if (fidx < 1 || fidx > FRAMES_COUNT) {
        tm_raise("tm_getframe:invalid fidx %d", fidx);
    }
    return tm->frames + fidx;
}

Object tm_getlocal(int fidx, int lidx) {
    TmFrame* f = tm_getframe(fidx);
    if (lidx < 0 || lidx >= f->maxlocals) {
        tm_raise("tm_getlocal:invalid lidx %d, maxlocals=%d", lidx, f->maxlocals);
    }
    return f->locals[lidx];
}

Object tm_getstack(int fidx, int sidx) {
    TmFrame* f = tm_getframe(fidx);
    int stacksize = f->top - f->stack;
    if (sidx < 0 || sidx >= stacksize) {
        tm_raise("tm_getstack:invalid sidx %d, stacksize=%d", sidx, stacksize);
    }
    return f->stack[sidx];
}

Object tm_getglobal(Object globals, Object okey) {
    DictNode* node = dict_get_node(GET_DICT(globals), okey);
    if (node == NULL) {
        node = dict_get_node(GET_DICT(tm->builtins), okey);
        if (node == NULL) {
            tm_raise("NameError: name %o is not defined", okey);
        }
    }
    return node->val;
}

Object tm_getfname(Object fnc) {
    if (!IS_FUNC(fnc)) {
        tm_raise("tm_getfname expect function");
    }
    return GET_MODULE(GET_FUNCTION(fnc)->mod)->file;
}

void tm_setattr(Object dict, char* attr, Object value) {
    dict_set(GET_DICT(dict), string_static(attr), value);
}

Object tm_call(Object func, int args, ...) {
    int i = 0;
    va_list ap;
    va_start(ap, args);
    arg_start();
    for (i = 0; i < args; i++) {
        arg_push(va_arg(ap, Object));
    }
    va_end(ap);
    return callFunction(func);
}

Object tm_string(char* str) {
    return string_static(str);
}

Object tm_list(int n, ...) {
    va_list ap;
    int i = 0;
    Object list = list_new(n);
    va_start(ap, n);
    for (i = 0; i < n; i++) {
        Object item = va_arg(ap, Object);
        tm_append(list, item);
    }
    va_end(ap);
    return list;
}

void tm_define(Object globals, Object name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = name;
    tm_set(globals,name, func);
}

void tm_method(Object dict, Object name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    Object method = method_new(func, dict);
    tm_set(dict, name, method);
}

Object tm_getarg() {
    return arg_get_obj("getarg");
}