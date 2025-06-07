/**
 * Object operator implementation
 * 对象相关的操作实现，这些函数的入参大部分都是`MpObj`或者`MpObj*`
 * 这个模块的作用是封装了对MpStr/MpList/MpDict等对象的调用
 * @author xupingmao
 * @since 2016
 * @modified 2022/06/11 18:49:48
 */

#include <assert.h>
#include "include/mp.h"

inline void mp_assert(int value, char* msg) {
    if (!value) {
        mp_raise("assertion failed, %s", msg);
    }
}

const char* mp_get_type_cstr(int type) {
    static char MP_TYPE_STR[64];

    switch (type) {
        case TYPE_STR:
            return "str";
        case TYPE_NUM:
            return "number";
        case TYPE_LIST:
            return "list";
        case TYPE_FUNCTION:
            return "function";
        case TYPE_DICT:
            return "dict";
        case TYPE_DATA:
            return "data";
        case TYPE_CLASS:
            return "class";
        case TYPE_NONE:
            return "None";
        case TYPE_MODULE:
            return "module";
    }
    sprintf(MP_TYPE_STR, "unknown(%d)", type);
    return MP_TYPE_STR;
}

inline const char* get_object_type_cstr(MpObj o) {
    return mp_get_type_cstr(o.type);
}

void mp_assert_type(MpObj o, int type, char* msg) {
    if (MP_TYPE(o) != type) {
        mp_raise("%s: expect %s but see %s", msg, mp_get_type_cstr(type),
                 mp_get_type_cstr(o.type));
    }
}

void mp_assert_type2(MpObj o, int type1, int type2, char* msg) {
    if (MP_TYPE(o) != type1 && MP_TYPE(o) != type2) {
        mp_raise("%s: expect %s or %s but see %s", msg, mp_get_type_cstr(type1),
                 mp_get_type_cstr(type2), mp_get_type_cstr(o.type));
    }
}

void mp_assert_int(double value, char* msg) {
    /* (int)(-1.2) = -1 */
    if (value - floor(value) > 0.000001) {
        mp_raise("%s require int", msg);
    }
}

int obj_eq_cstr(MpObj obj, const char* value) {
    return MP_TYPE(obj) == TYPE_STR &&
           (GET_CSTR(obj) == value || strcmp(GET_CSTR(obj), value) == 0);
}

void obj_set(MpObj self, MpObj k, MpObj v) {
    // gc_mark_single(k); // used between gc scan
    // gc_mark_single(v); // only need to mark single.
    switch (MP_TYPE(self)) {
        case TYPE_LIST: {
            mp_assert_type(k, TYPE_NUM, "obj_set");
            double d = GET_NUM(k);
            mp_assert_int(d, "list_set");
            list_set(GET_LIST(self), (int)d, v);
            return;
        }
        case TYPE_DICT:
            dict_set0(GET_DICT(self), k, v);
            return;
        case TYPE_CLASS: {
            MpClass* pclass = GET_CLASS(self);
            // dict_set0(pclass->attr_dict, k, v);
            class_set_attr(pclass, k, v);
            return;
        }
        case TYPE_INSTANTCE: {
            MpInstance* instance = GET_INSTANCE(self);
            assert(instance->klass != NULL);
            assert(instance->klass->setattr_method.type == TYPE_FUNCTION);

            MpObj args[3] = {self, k, v};
            mp_call_obj_safe(instance->klass->setattr_method, 3, args);
            return;
        }
    }
    mp_raise("obj_set: Self %o, Key %o, Val %o", self, k, v);
}

inline void obj_set_by_cstr(MpObj self, char* key, MpObj value) {
    obj_set(self, string_new(key), value);
}

MpObj mp_getattr(MpObj self, MpObj k) {
    MpObj v;
    switch (MP_TYPE(self)) {
        case TYPE_STR: {
            DictNode* node;
            if (MP_TYPE(k) == TYPE_NUM) {
                double d = GET_NUM(k);
                int n = d;
                if (n < 0) {
                    n += GET_STR_LEN(self);
                }
                if (n >= GET_STR_LEN(self) || n < 0)
                    mp_raise(
                        "MpStr_get: index overflow ,len=%d,index=%d, str=%o",
                        GET_STR_LEN(self), n, self);
                return string_chr(0xff & GET_CSTR(self)[n]);
            }
            // string method
            if ((node = dict_get_node(GET_DICT(tm->str_proto), k)) != NULL) {
                return method_new(node->val, self);
            }
            break;
        }
        case TYPE_LIST: {
            DictNode* node;
            if (MP_TYPE(k) == TYPE_NUM) {
                return list_get(GET_LIST(self), GET_NUM(k));
            }
            // list method
            if ((node = dict_get_node(GET_DICT(tm->list_proto), k)) != NULL) {
                return method_new(node->val, self);
            }
            break;
        }
        case TYPE_DICT: {
            DictNode* node;
            node = dict_get_node(GET_DICT(self), k);
            if (node != NULL) {
                return node->val;
            }
            // dict method
            if ((node = dict_get_node(GET_DICT(tm->dict_proto), k)) != NULL) {
                return method_new(node->val, self);
            }
            break;
        }
        case TYPE_FUNCTION:
            return func_get_attr(GET_FUNCTION(self), k);
        case TYPE_DATA:
            return GET_DATA(self)->get(GET_DATA(self), k);
        case TYPE_CLASS: {
            MpClass* pclass = GET_CLASS(self);
            DictNode *node = dict_get_node(pclass->attr_dict, k);
            if (node != NULL) {
                return node->val;
            }
        }
        case TYPE_INSTANTCE: {
            MpInstance* instance = GET_INSTANCE(self);
            assert(instance->klass != NULL);
            assert(instance->klass->getattr_method.type == TYPE_FUNCTION);

            MpObj args[2] = {self, k};
            return mp_call_obj_safe(instance->klass->getattr_method, 2, args);
        }
    }
    mp_raise("keyError: %o", k);
    return NONE_OBJECT;
}

MpObj obj_get_by_cstr(MpObj obj, char* key) {
    return mp_getattr(obj, string_new(key));
}

/**
 * slice string/list
 * string.slice(a,b) [a,b), b is not included
 * string.slice(0,-1) = string.slice(0,len(string)-1)
 * @since 2016-11-19
 * <code>
 * 'test'.slice(0,1) = 't'
 * 'test'.slice(0,-1) = 'tes'
 * </code>
 */
MpObj obj_slice(MpObj self, MpObj first, MpObj second) {
    int start = GET_NUM(first);
    int end = 0;
    if (IS_NONE(second)) {
        end = mp_len(self);
    } else {
        end = GET_NUM(second);
    }

    MpObj ret = NONE_OBJECT;

    if (IS_STR(self)) {
        int length = GET_STR_LEN(self);
        start = start >= 0 ? start : start + length;
        end = end >= 0 ? end : end + length;
        if (start < 0 || start > length) {
            start = 0;
        }
        if (end < 0 || end > length) {
            end = length;  // do not overflow;
        }
        if (end <= start) {
            return string_alloc("", 0);
        }
        return string_alloc(GET_CSTR(self) + start, end - start);
    } else if (IS_LIST(self)) {
        int length = LIST_LEN(self);
        if (start < 0) {
            start += length;
        }
        if (end < 0) {
            end += length;
        }
        if (start < 0 || start > length) {
            start = 0;
        }
        if (end < 0 || end > length) {
            end = length;
        }
        int i = 0;
        ret = list_new(end - start);
        for (i = start; i < end; i++) {
            mp_append(ret, LIST_GET(self, i));
        }
    } else {
        mp_raise("slice not implemented for type %s", mp_get_type_cstr(self.type));
    }
    return ret;
}

MpObj obj_sub(MpObj a, MpObj b) {
    if (a.type == b.type) {
        if (a.type == TYPE_NUM) {
            GET_NUM(a) -= GET_NUM(b);
            return a;
        }
    }
    mp_raise("obj_sub: can not substract %o and %o", a, b);
    return NONE_OBJECT;
}

MpObj obj_add(MpObj a, MpObj b) {
    if (MP_TYPE(a) == MP_TYPE(b)) {
        switch (MP_TYPE(a)) {
            case TYPE_NUM:
                GET_NUM(a) += GET_NUM(b);
                return a;
            case TYPE_STR: {
                return string_add(GET_STR_OBJ(a), GET_STR_OBJ(b));
            }
            case TYPE_LIST: {
                return list_add(GET_LIST(a), GET_LIST(b));
            }
        }
    }
    mp_raise("obj_add: can not add %o and %o", (a), (b));
    return NONE_OBJECT;
}

int mp_is_equals(MpObj a, MpObj b) {
    if (MP_TYPE(a) != MP_TYPE(b)) {
        return FALSE;
    }

    switch (MP_TYPE(a)) {
        case TYPE_NUM: {
            return GET_NUM(a) == GET_NUM(b);
        }
        case TYPE_STR: {
            MpStr* s1 = GET_STR_OBJ(a);
            MpStr* s2 = GET_STR_OBJ(b);
            return (s1 == s2) || (s1->value == s2->value) ||
                   (s1->len == s2->len &&
                    strncmp(s1->value, s2->value, s1->len) == 0);
        }
        case TYPE_LIST: {
            if (GET_LIST(a) == GET_LIST(b))
                return 1;
            if (LIST_LEN(a) != LIST_LEN(b))
                return 0;
            int i;
            int len = GET_LIST(a)->len;
            MpObj* nodes1 = GET_LIST(a)->nodes;
            MpObj* nodes2 = GET_LIST(b)->nodes;
            for (i = 0; i < len; i++) {
                if (!mp_is_equals(nodes1[i], nodes2[i])) {
                    return 0;
                }
            }
            return TRUE;
        }
        case TYPE_NONE:
            return 1;
        case TYPE_DICT:
            return GET_DICT(a) == GET_DICT(b);
        case TYPE_FUNCTION:
            return GET_FUNCTION(a) == GET_FUNCTION(b);
        case TYPE_CLASS:
            return GET_CLASS(a) == GET_CLASS(b);
        default: {
            const char* ltype = mp_get_type_cstr(a.type);
            const char* rtype = mp_get_type_cstr(b.type);
            mp_raise("mp_is_equals: not supported type %d:%s and %d:%s",
                     MP_TYPE(a), ltype, MP_TYPE(b), rtype);
        }
    }
    return FALSE;
}

MpObj mp_cmp_as_obj(MpObj a, MpObj b) {
    return mp_number(mp_cmp(a, b));
}

int mp_cmp(MpObj a, MpObj b) {
    if (MP_TYPE(a) == MP_TYPE(b)) {
        switch (MP_TYPE(a)) {
            case TYPE_NUM: {
                double diff = GET_NUM(a) - GET_NUM(b);
                if (diff > 0.0) {
                    return 1;
                } else if (diff < 0.0) {
                    return -1;
                }
                return 0;
            }
            case TYPE_STR:
                return strcmp(GET_CSTR(a), GET_CSTR(b));
        }
    }
    mp_raise("mp_cmp: can not compare %o and %o", a, b);
    return 0;
}

MpObj obj_mul(MpObj a, MpObj b) {
    /* number * number */
    if (a.type == b.type && a.type == TYPE_NUM) {
        GET_NUM(a) *= GET_NUM(b);
        return a;
    }
    /* number * string */
    if (a.type == TYPE_NUM && b.type == TYPE_STR) {
        MpObj temp = a;
        a = b;
        b = temp;
    }
    if (a.type == TYPE_STR && b.type == TYPE_NUM) {
        int len = GET_STR_LEN(a);
        MpObj des;
        if (len == 0)
            return a;
        int times = (int)GET_NUM(b);
        if (times <= 0)
            return string_from_cstr("");
        if (times == 1)
            return a;
        des = string_alloc(NULL, len * times);
        char* s = GET_CSTR(des);
        int i;
        for (i = 0; i < times; i++) {
            strncpy(s, GET_CSTR(a), len);
            s += len;
        }
        return des;
    }
    /* list * number */
    if (a.type == TYPE_NUM && b.type == TYPE_LIST) {
        MpObj temp = a;
        a = b;
        b = temp;
    }
    if (a.type == TYPE_LIST && b.type == TYPE_NUM) {
        if (GET_NUM(b) < 1) {
            return list_new(0);
        }
        MpObj new_list = list_new(LIST_LEN(a) * GET_NUM(b));
        int i = 0;
        for (i = 0; i < GET_NUM(b); i++) {
            mp_call_builtin(list_builtin_extend, 2, new_list, a);
        }
        return new_list;
    }
    mp_raise("obj_mul: can not multiply %o and %o", a, b);
    return NONE_OBJECT;
}

MpObj obj_div(MpObj a, MpObj b) {
    if (a.type == b.type && a.type == TYPE_NUM) {
        double bv = GET_NUM(b);
        if (bv == 0.0f) {
            mp_raise("ZeroDivisionError: float division by zero");
        }
        GET_NUM(a) /= GET_NUM(b);
        return a;
    }
    mp_raise("obj_div: can not divide %o and %o", a, b);
    return NONE_OBJECT;
}

MpObj obj_mod(MpObj a, MpObj b) {
    if (a.type == b.type && a.type == TYPE_NUM) {
        return mp_number((long)GET_NUM(a) % (long)GET_NUM(b));
    } else if (a.type == TYPE_STR) {
        MpObj* __mod__ = mp_get_builtin("__mod__");
        if (__mod__ == NULL) {
            return string_ops_mod(a, b);
        } else {
            mp_reset_args();
            mp_push_arg(a);
            mp_push_arg(b);
            return MP_CALL_EX(*__mod__);
        }
    }
    mp_raise("obj_mod: can not module %o and %o", a, b);
    return NONE_OBJECT;
}

/* parent has child
 * child in parent
 */
int mp_is_in(MpObj child, MpObj parent) {
    switch (MP_TYPE(parent)) {
        case TYPE_LIST: {
            return (list_index(GET_LIST(parent), child) != -1);
        }
        case TYPE_STR: {
            if (MP_TYPE(child) != TYPE_STR)
                return 0;
            return string_index(GET_STR_OBJ(parent), GET_STR_OBJ(child), 0) !=
                   -1;
        }
        case TYPE_DICT: {
            DictNode* node = dict_get_node(GET_DICT(parent), child);
            if (node == NULL) {
                return 0;
            }
            return 1;
        }
        case TYPE_NONE:
            return 0;
        case TYPE_NUM:
            return 0;
        case TYPE_FUNCTION:
            return 0;
        case TYPE_INSTANTCE:
            return mp_is_in_instance(GET_INSTANCE(parent), child);
        /* TODO DATA */
        default:
            mp_raise("obj_is_in: cant handle type (%s)",
                     mp_get_type_cstr(MP_TYPE(parent)));
    }
    return 0;
}

int mp_is_not_in(MpObj child, MpObj parent) {
    return !mp_is_in(child, parent);
}

MpObj obj_is_in(MpObj left, MpObj right) {
    return mp_number(mp_is_in(left, right));
}

int mp_is_true(MpObj v) {
    switch (MP_TYPE(v)) {
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
        case TYPE_CLASS:
        case TYPE_DATA:
            return 1;
        case TYPE_INSTANTCE:
            return 1;
        default:
            mp_raise("mp_is_true: unknown type(%d)", MP_TYPE(v));
    }
    return 0;
}

MpObj obj_neg(MpObj o) {
    if (o.type == TYPE_NUM) {
        GET_NUM(o) = -GET_NUM(o);
        return o;
    }
    mp_raise("TypeError: bad operand type for unary -: %ot", o);
    return NONE_OBJECT;
}

MpObj obj_or(MpObj a, MpObj b) {
    return mp_number(mp_is_true(a) || mp_is_true(b));
}

MpObj iter_new(MpObj collections) {
    switch (MP_TYPE(collections)) {
        case TYPE_LIST:
            return list_iter_new(collections);
        case TYPE_DICT:
            return dict_iter_new(collections);
        case TYPE_CLASS:
            return iter_new(mp_to_obj(TYPE_DICT, GET_CLASS(collections)->attr_dict));
        case TYPE_STR:
            return string_iter_new(collections);
        case TYPE_DATA:
            return collections;
        default:
            mp_raise("iter_new: can not create a iterator of %o", collections);
    }
    return NONE_OBJECT;
}

MpObj* mp_next(MpObj iterator) {
    return GET_DATA(iterator)->next(GET_DATA(iterator));
}

void obj_del(MpObj self, MpObj k) {
    switch (MP_TYPE(self)) {
        case TYPE_DICT: {
            dict_del(GET_DICT(self), k);
            break;
        }
        case TYPE_LIST: {
            list_del(GET_LIST(self), k);
            break;
        }
        default:
            mp_raise("obj_del: not supported type %s",
                     mp_get_type_cstr(self.type));
    }
}

MpObj mp_append(MpObj a, MpObj item) {
    if (IS_LIST(a)) {
        list_append(GET_LIST(a), item);
    } else {
        mp_raise("mp_append: not supported type %s", mp_get_type_cstr(a.type));
    }
    return a;
}

MpObj mp_get_global_by_cstr(MpObj globals, char* key) {
    mp_assert_type(globals, TYPE_DICT, "mp_get_global_by_cstr");
    MpObj okey = string_new(key);
    DictNode* node = dict_get_node(GET_DICT(globals), okey);
    if (node == NULL) {
        node = dict_get_node(tm->builtins, okey);
        if (node == NULL) {
            mp_raise("NameError: name %o is not defined", okey);
        }
    }
    return node->val;
}

int mp_len(MpObj o) {
    int len = -1;
    switch (MP_TYPE(o)) {
        case TYPE_STR:
            len = GET_STR_LEN(o);
            break;
        case TYPE_LIST:
            len = LIST_LEN(o);
            break;
        case TYPE_DICT:
            len = DICT_LEN(o);
            break;
    }
    if (len < 0) {
        mp_raise("mp_len: %o has no attribute len", o);
    }
    return len;
}

const char* mp_to_cstr(MpObj a) {
    MpObj b = mp_str(a);
    return GET_CSTR(b);
}

// func MpObj mp_str(MpObj a)
MpObj mp_str(MpObj a) {
    char buf[100];
    memset(buf, 0, sizeof(buf));
    switch (MP_TYPE(a)) {
        case TYPE_STR:
            return a;
        case TYPE_NUM: {
            char s[20];
            double v = GET_NUM(a);
            mp_format_number(s, a);
            return string_new(s);
        }
        case TYPE_LIST: {
            MpObj str = string_new("");

            string_append_char(str, '[');
            int i, l = LIST_LEN(a);
            for (i = 0; i < l; i++) {
                MpObj obj = GET_LIST(a)->nodes[i];
                /* reference to self in list */
                if (mp_is_equals(a, obj)) {
                    string_append_cstr(str, "[...]");
                } else if (obj.type == TYPE_STR) {
                    string_append_char(str, '"');
                    string_append_obj(str, obj);
                    string_append_char(str, '"');
                } else {
                    string_append_obj(str, obj);
                }
                if (i != l - 1) {
                    string_append_char(str, ',');
                }
            }
            string_append_char(str, ']');
            return str;
        }
        case TYPE_DICT:
            sprintf(buf, "<dict at %p>", GET_DICT(a));
            return string_new(buf);
        case TYPE_FUNCTION:
            FUNCTION_FORMAT(buf, a);
            return string_new(buf);
        case TYPE_CLASS:
            return mp_format_class(GET_CLASS(a));
        case TYPE_NONE:
            return string_static("None");
        case TYPE_MODULE: {
            MpModule* module = GET_MODULE(a);
            assert(module->file != NULL);
            sprintf(buf, "<module %s at %p>", module->file->value, module);
            return string_new(buf);
        }
        case TYPE_DATA:
            return GET_DATA(a)->str(GET_DATA(a));
        case TYPE_INSTANTCE:
            return mp_format_instance(GET_INSTANCE(a));
        case TYPE_PTR:
            sprintf(buf, "<ptr at %p>", GET_PTR(a));
            return string_new(buf);
        default:
            sprintf(buf, "<unknown(%d)>", a.type);
            return string_new(buf);
            // mp_raise("str: not supported type %d", a.type);
    }
    return string_alloc("", 0);
}

/** get const id, this will be used to search the const value */
int get_const_id(MpObj const_value) {
    int i = dict_set0(tm->constants, const_value, NONE_OBJECT);
    return i;
}

MpObj mp_get_constant(int index) {
    assert(index >= 0);
    assert(index < tm->constants->len);
    return tm->constants->nodes[index].key;
}

MpObj mp_call_builtin(BuiltinFunc func, int n, ...) {
    int i = 0;
    va_list ap;
    va_start(ap, n);

    mp_reset_args();
    for (i = 0; i < n; i++) {
        mp_push_arg(va_arg(ap, MpObj));
    }
    return func();
}

MpObj mp_get_globals(MpObj fnc) {
    mp_assert_type(fnc, TYPE_FUNCTION, "mp_get_globals");
    return func_get_globals(GET_FUNCTION(fnc));
}

MpObj mp_get_globals_from_module(MpObj module) {
    mp_assert_type(module, TYPE_MODULE, "mp_get_globals_from_module");
    return GET_MODULE(module)->globals;
}


/**
 * create new object
 * @param type object type
 * @param value object pointer
 * @value object pointer
 */
MpObj mp_to_obj(int type, void* value) {
    MpObj o;
    MP_TYPE(o) = type;
    switch (type) {
        case TYPE_NUM:
            o.value.num = *(double*)value;
            break;
        case TYPE_STR:
            o.value.str = value;
            break;
        case TYPE_LIST:
            GET_LIST(o) = value;
            break;
        case TYPE_DICT:
            GET_DICT(o) = value;
            break;
        case TYPE_MODULE:
            GET_MODULE(o) = value;
            break;
        case TYPE_FUNCTION:
            GET_FUNCTION(o) = value;
            break;
        case TYPE_CLASS:
            GET_CLASS(o) = value;
            break;
        case TYPE_NONE:
            break;
        case TYPE_INSTANTCE:
            GET_INSTANCE(o) = value;
            break;
        default:
            mp_raise("mp_to_obj: not supported type %d", type);
    }
    return o;
}


/**
 * get attribute by char array
 * @since 2016-09-02
 */
MpObj mp_getattr_by_cstr(MpObj obj, const char* key) {
    // TODO optimize string find
    MpObj obj_key = string_static(key);
    return mp_getattr(obj, obj_key);
}


/**
 * get attribute by char array
 * @since 2016-09-02
 */
void mp_setattr(MpObj obj, const char* key, MpObj value) {
    MpObj obj_key = string_static(key);
    obj_set(obj, obj_key, value);
}

int mp_hasattr(MpObj obj, const char* key) {
    mp_assert_type(obj, TYPE_DICT, "mp_hasattr");

    MpObj obj_key = string_static(key);
    DictNode* node = dict_get_node(GET_DICT(obj), obj_key);
    if (node == NULL) {
        return 0;
    }
    return 1;
}
