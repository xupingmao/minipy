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

void objSet(Object self, Object k, Object v) {
    switch (TM_TYPE(self)) {
    case TYPE_LIST: {
        tmAssertType(k, TYPE_NUM, "objSet");
        double d = GET_NUM(k);
        tmAssertInt(d, "listSet");
        list_set(GET_LIST(self), (int)d, v);
    }
        return;
    case TYPE_DICT:
        dict_set(GET_DICT(self), k, v);
        return;
    }
    tm_raise("objSet: Self %o, Key %o, Val %o", self, k, v);
}

Object objGet(Object self, Object k) {
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
        } else if ((node = dictGetNode(GET_DICT(tm->str_proto), k)) != NULL) {
            return methodNew(node->val, self);
        }
        break;
    }
    case TYPE_LIST:{
        DictNode* node;
        if (TM_TYPE(k) == TYPE_NUM) {
            return list_get(GET_LIST(self), GET_NUM(k));
        } else if ((node = dictGetNode(GET_DICT(tm->list_proto), k))!=NULL) {
            return methodNew(node->val, self);
        }
        break;
    }
    case TYPE_DICT:{
        DictNode* node;
        node = dictGetNode(GET_DICT(self), k);
        if (node != NULL) {
            return node->val;
        } else if ((node = dictGetNode(GET_DICT(tm->dict_proto), k))!=NULL) {
            return methodNew(node->val, self);
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

Object objSub(Object a, Object b) {
    if (a.type == b.type) {
        if (a.type == TYPE_NUM) {
            GET_NUM(a) -= GET_NUM(b);
            SET_IDX(a, 0);
            return a;
        }
    }
    tm_raise("objSub: can not substract %o and %o", a, b);
    return NONE_OBJECT;
}

Object objAdd(Object a, Object b) {
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
            Object des = stringAlloc(NULL, len);
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
    tm_raise("objAdd: can not add %o and %o", (a), (b));
    return NONE_OBJECT;
}

int objEquals(Object a, Object b){
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
                if(!objEquals(nodes1[i], nodes2[i]) ){
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

int objCmp(Object a, Object b) {
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
    tm_raise("objCmp: can not compare %o and %o", a, b);
    return 0;
}
/*
DEF_CMP_FUNC_2(isTrueObj_lt, <);
DEF_CMP_FUNC_2(isTrueObj_gt, >);
DEF_CMP_FUNC_2(isTrueObj_lteq, <=);
DEF_CMP_FUNC_2(isTrueObj_gteq, >=);
*/

Object objMul(Object a, Object b) {
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
            return szToString("");
        if (times == 1)
            return a;
        des = stringAlloc(NULL, len * times);
        char* s = GET_STR(des);
        int i;
        for (i = 0; i < times; i++) {
            strncpy(s, GET_STR(a), len);
            s += len;
        }
        return des;
    }
    tm_raise("objMul: can not multiply %o and %o", a, b);
    return NONE_OBJECT;
}

Object objDiv(Object a, Object b) {
    if (a.type == b.type && a.type == TYPE_NUM) {
        GET_NUM(a) /= GET_NUM(b);
        SET_IDX(a, 0);
        return a;
    }
    tm_raise("objDiv: can not divide %o and %o", a, b);
    return NONE_OBJECT;
}

Object objMod(Object a, Object b) {
    if (a.type == b.type && a.type == TYPE_NUM) {
        return tmNumber((long) GET_NUM(a) % (long) GET_NUM(b));
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
    tm_raise("objMod: can not module %o and %o", a, b);
    return NONE_OBJECT;
}

/* a has b */
int objHas(Object a, Object b) {
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
        DictNode* node = dictGetNode(GET_DICT(a), b);
        if (node == NULL) {
            return 0;
        }
        return 1;
    }
    }
    return 0;
}
int isTrueObj(Object v) {
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

Object objNeg(Object o) {
    if (o.type == TYPE_NUM) {
        GET_NUM(o) = -GET_NUM(o);
        return o;
    }
    tm_raise("objNeg: can not handle %o", o);
    return NONE_OBJECT;
}


Object iterNew(Object collections) {
    switch(TM_TYPE(collections)) {
        case TYPE_LIST: return listIterNew(GET_LIST(collections));
        case TYPE_DICT: return dict_iterNew(GET_DICT(collections));
        case TYPE_STR: return stringIterNew(GET_STR_OBJ(collections));
        case TYPE_DATA: return collections;
        default: tm_raise("iterNew(): can not create a iterator of %o", collections);
    }
    return NONE_OBJECT;
}

Object* nextPtr(Object iterator) {
    return GET_DATA_PROTO(iterator)->next(GET_DATA(iterator));
}

void objDel(Object self, Object k) {
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
            tm_raise("objDel: not supported type %s", getTypeByObj(self));
    }
}

Object objAppend(Object a, Object item) {
    if (IS_LIST(a)) {
        listAppend(GET_LIST(a), item);
    }
    return a;
}

TmFrame* objGetframe(int fidx) {
    if (fidx < 1 || fidx > FRAMES_COUNT) {
        tm_raise("objGetframe:invalid fidx %d", fidx);
    }
    return tm->frames + fidx;
}

Object objGetlocal(int fidx, int lidx) {
    TmFrame* f = objGetframe(fidx);
    if (lidx < 0 || lidx >= f->maxlocals) {
        tm_raise("objGetlocal:invalid lidx %d, maxlocals=%d", lidx, f->maxlocals);
    }
    return f->locals[lidx];
}

Object objGetstack(int fidx, int sidx) {
    TmFrame* f = objGetframe(fidx);
    int stacksize = f->top - f->stack;
    if (sidx < 0 || sidx >= stacksize) {
        tm_raise("objGetstack:invalid sidx %d, stacksize=%d", sidx, stacksize);
    }
    return f->stack[sidx];
}

Object tmGetGlobal(Object globals, Object okey) {
    DictNode* node = dictGetNode(GET_DICT(globals), okey);
    if (node == NULL) {
        node = dictGetNode(GET_DICT(tm->builtins), okey);
        if (node == NULL) {
            tm_raise("NameError: name %o is not defined", okey);
        }
    }
    return node->val;
}

Object tmGetfname(Object fnc) {
    if (!IS_FUNC(fnc)) {
        tm_raise("tmGetfname expect function");
    }
    return GET_MODULE(GET_FUNCTION(fnc)->mod)->file;
}

void tmSetattr(Object dict, char* attr, Object value) {
    dict_set(GET_DICT(dict), szToString(attr), value);
}

Object tmCall(Object func, int args, ...) {
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
    return szToString(str);
}

Object arrayToList(int n, ...) {
    va_list ap;
    int i = 0;
    Object list = listNew(n);
    va_start(ap, n);
    for (i = 0; i < n; i++) {
        Object item = va_arg(ap, Object);
        objAppend(list, item);
    }
    va_end(ap);
    return list;
}

void defFunc(Object globals, Object name, Object (*native)()) {
    Object func = funcNew(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = name;
    objSet(globals,name, func);
}

void defMethod(Object dict, Object name, Object (*native)()) {
    Object func = funcNew(NONE_OBJECT, NONE_OBJECT, native);
    Object method = methodNew(func, dict);
    objSet(dict, name, method);
}

Object tmTakeArg() {
    return arg_get_obj("getarg");
}

void tmDefMod(char* fname, Object mod) {
    Object oName = szToString(fname);
    objSet(tm->modules, oName, mod);
}

void tmImportAll(Object globals, Object modName) {
    int bHas = objHas(tm->modules, modName);
    if (bHas) {
        Object modValue = objGet(tm->modules, modName);
        // do something here.
    }
}

