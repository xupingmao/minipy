/**
 * opeartor implementions
 * @author xupingmao
 * @since 2016
 * @modified 2020/10/23 00:35:36
 */

#include <assert.h>
#include "include/mp.h"

void mp_assert(int value, char* msg) {
    if (!value) {
        mp_raise("assertion failed, %s", msg);
    }
}

const char* mp_type(int type) {
    static char MP_TYPE_STR[64];

    switch (type) {
        case TYPE_STR:
            return "string";
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

void mp_assert_type(MpObj o, int type, char* msg) {
    if (MP_TYPE(o) != type) {
        mp_raise("%s, expect %s but see %s", msg, 
            mp_type(type), mp_type(o.type));
    }
}

void mp_assert_type2(MpObj o, int type1, int type2, char* msg) {
    if (MP_TYPE(o) != type1 && MP_TYPE(o) != type2) {
        mp_raise("%s, expect %s or %s but see %s", msg, 
            mp_type(type1), mp_type(type2), mp_type(o.type));
    }
}

void mp_assert_int(double value, char* msg) {
    /* (int)(-1.2) = -1 */
    if (value - floor(value) > 0.000001) {
        mp_raise("%s require int", msg);
    }
}

int obj_eq_sz(MpObj obj, const char* value) {
    return MP_TYPE(obj) == TYPE_STR
            && (GET_STR(obj) == value || strcmp(GET_STR(obj), value) == 0);
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
            dict_set0(GET_DICT(pclass->attr_dict), k, v);
            return;
        }
    }
    mp_raise("obj_set: Self %o, Key %o, Val %o", self, k, v);
}

MpObj obj_get(MpObj self, MpObj k) {
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
                    mp_raise("MpStr_get: index overflow ,len=%d,index=%d, str=%o",
                            GET_STR_LEN(self), n, self);
                return string_chr(0xff & GET_STR(self)[n]);
            }
            // string method
            if ((node = dict_get_node(GET_DICT(tm->str_proto), k)) != NULL) {
                return method_new(node->val, self);
            }
            break;
        }
        case TYPE_LIST:{
            DictNode* node;
            if (MP_TYPE(k) == TYPE_NUM) {
                return list_get(GET_LIST(self), GET_NUM(k));
            }
            // list method
            if ((node = dict_get_node(GET_DICT(tm->list_proto), k))!=NULL) {
                return method_new(node->val, self);
            }
            break;
        }
        case TYPE_DICT:{
            DictNode* node;
            node = dict_get_node(GET_DICT(self), k);
            if (node != NULL) {
                return node->val;
            } 
            // dict method
            if ((node = dict_get_node(GET_DICT(tm->dict_proto), k))!=NULL) {
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
            return obj_get(pclass->attr_dict, k);
        }
    }
    mp_raise("keyError %o", k);
    return NONE_OBJECT;
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
        end   = end   >= 0 ? end   : end   + length;
        if (start < 0 || start > length) {
            start = 0;
        } 
        if (end < 0 || end > length) {
            end = length; // do not overflow;
        } 
        if (end <= start) {
            return string_alloc("", 0);
        }
        return string_alloc(GET_STR(self) + start, end - start);
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
        for (i = start; i < end ; i++) {
            obj_append(ret, LIST_GET(self, i));
        }
    } else {
        mp_raise("slice not implemented for type %s", mp_type(self.type));
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
                char* sa = GET_STR(a);
                char* sb = GET_STR(b);
                int la = GET_STR_LEN(a);
                int lb = GET_STR_LEN(b);
                if (la == 0) {return b;    }
                if (lb == 0) {return a;    }
                int len = la + lb;
                MpObj des = string_alloc(NULL, len);
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
    mp_raise("obj_add: can not add %o and %o", (a), (b));
    return NONE_OBJECT;
}

int obj_equals(MpObj a, MpObj b){
    if(MP_TYPE(a) != MP_TYPE(b)) return 0;
    switch(MP_TYPE(a)){
        case TYPE_NUM:return GET_NUM(a) == GET_NUM(b);
        case TYPE_STR: {
            MpStr* s1 = GET_STR_OBJ(a);
            MpStr* s2 = GET_STR_OBJ(b);
            return s1->value == s2->value || 
                (s1->len == s2->len && strncmp(s1->value, s2->value, s1->len) == 0);
        }
        case TYPE_LIST:    {
            if(GET_LIST(a) == GET_LIST(b)) return 1;
            if(LIST_LEN(a) != LIST_LEN(b)) return 0;
            int i;
            int len = GET_LIST(a)->len;
            MpObj* nodes1 = GET_LIST(a)->nodes;
            MpObj* nodes2 = GET_LIST(b)->nodes;
            for(i = 0; i < len; i++){
                if(!obj_equals(nodes1[i], nodes2[i]) ){
                    return 0;
                }
            }
            return 1;
        }
        case TYPE_NONE:return 1;
        case TYPE_DICT:return GET_DICT(a) == GET_DICT(b);
        case TYPE_FUNCTION: return GET_FUNCTION(a) == GET_FUNCTION(b);
        case TYPE_CLASS: return GET_CLASS(a) == GET_CLASS(b);
        default: {
            const char* ltype = mp_type(a.type);
            const char* rtype = mp_type(b.type);
            mp_raise("obj_equals: not supported type %d:%s and %d:%s", MP_TYPE(a), ltype, MP_TYPE(b), rtype);
        } 
    }
    return 0;
}

MpObj obj_cmp(MpObj a, MpObj b) {
    return number_obj(mp_cmp(a, b));
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
            case TYPE_STR: return strcmp(GET_STR(a), GET_STR(b));
        }
    }
    mp_raise("obj_cmp: can not compare %o and %o", a, b);
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
        int times = (int) GET_NUM(b);
        if (times <= 0)
            return string_from_sz("");
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
        GET_NUM(a) /= GET_NUM(b);
        return a;
    }
    mp_raise("obj_div: can not divide %o and %o", a, b);
    return NONE_OBJECT;
}

MpObj string_mod_list(MpObj str, MpObj list) {
    assert(MP_TYPE(str)  == TYPE_STR);
    assert(MP_TYPE(list) == TYPE_LIST);

    char* fmt = GET_SZ(str);
    int str_length = GET_STR_LEN(str);
    MpList* plist = GET_LIST(list);
    int i = 0;
    int arg_index = 0;

    MpObj result = string_new("");

    for (i = 0; i < str_length; i++) {
        char c = fmt[i];

        if (c == '%') {
            i++;
            switch(fmt[i]) {
                case 's':
                    string_append_obj(result, list_get(plist, arg_index));
                    arg_index++;
                    break;
                case 'd': {
                    MpObj item = list_get(plist, arg_index);                 
                    mp_assert_type(item, TYPE_NUM, "obj_mod");  
                    string_append_obj(result, item);
                    arg_index++;
                    break;
                }
                default:
                    mp_raise("obj_mod: unknown format type %c", fmt[i]);
            }
        } else {
            string_append_char(result, c);
        }
    }

    return result;
}

MpObj string_ops_mod(MpObj a, MpObj b) {
    assert(MP_TYPE(a) == TYPE_STR);
    char* fmt = GET_SZ(a);

    if (MP_TYPE(b) == TYPE_LIST) {
        return string_mod_list(a, b);
    } else {
        MpObj list = list_new(1);
        MpList* plist = GET_LIST(list);
        list_append(plist, b);
        return string_mod_list(a, list);
    }
}

MpObj obj_mod(MpObj a, MpObj b) {
    if (a.type == b.type && a.type == TYPE_NUM) {
        return number_obj((long) GET_NUM(a) % (long) GET_NUM(b));
    } else if (a.type == TYPE_STR) {
        MpObj *__mod__ = get_builtin("__mod__");
        if (__mod__ == NULL) {
            return string_ops_mod(a, b);
        } else {
            arg_start();
            arg_push(a);
            arg_push(b);
            return call_function(*__mod__);
        }        
    }
    mp_raise("obj_mod: can not module %o and %o", a, b);
    return NONE_OBJECT;
}


/* parent has child
 * child in parent
 */
int mp_in(MpObj child, MpObj parent) {
    switch (MP_TYPE(parent)) {
        case TYPE_LIST: {
            return (list_index(GET_LIST(parent), child) != -1);
        }
        case TYPE_STR: {
            if (MP_TYPE(child) != TYPE_STR)
                return 0;
            return string_index(GET_STR_OBJ(parent), GET_STR_OBJ(child), 0) != -1;
        }
        case TYPE_DICT: {
            DictNode* node = dict_get_node(GET_DICT(parent), child);
            if (node == NULL) {
                return 0;
            }
            return 1;
        }
        case TYPE_NONE: return 0;
        case TYPE_NUM:  return 0;
        case TYPE_FUNCTION: return 0;
        /* TODO DATA */ 
        default: mp_raise("obj_in: cant handle type (%s)", mp_type(MP_TYPE(parent)));
    }
    return 0;
}

int obj_not_in(MpObj child, MpObj parent) {
    return !mp_in(child, parent);
}

MpObj obj_in(MpObj left, MpObj right) {
    return number_obj(mp_in(left, right));
}

int is_true_obj(MpObj v) {
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
    case TYPE_DATA:
        return 1;
    }
    return 0;
}

MpObj obj_neg(MpObj o) {
    if (o.type == TYPE_NUM) {
        GET_NUM(o) = -GET_NUM(o);
        return o;
    }
    mp_raise("obj_neg: can not handle %o", o);
    return NONE_OBJECT;
}


MpObj iter_new(MpObj collections) {
    switch(MP_TYPE(collections)) {
        case TYPE_LIST: return list_iter_new(collections);
        case TYPE_DICT: return dict_iter_new(collections);
        case TYPE_CLASS: return iter_new(GET_CLASS(collections)->attr_dict);
        case TYPE_STR:  return string_iter_new(collections);
        case TYPE_DATA: return collections;
        default: mp_raise("iter_new(): can not create a iterator of %o", collections);
    }
    return NONE_OBJECT;
}

MpObj* next_ptr(MpObj iterator) {
    return GET_DATA(iterator)->next(GET_DATA(iterator));
}

void obj_del(MpObj self, MpObj k) {
    switch(MP_TYPE(self)) {
        case TYPE_DICT:{
            dict_del(GET_DICT(self), k);
            break;
        }
        case TYPE_LIST:{
            list_del(GET_LIST(self), k);
            break;
        }
        default:
            mp_raise("obj_del: not supported type %s", mp_type(self.type));
    }
}

MpObj obj_append(MpObj a, MpObj item) {
    if (IS_LIST(a)) {
        list_append(GET_LIST(a), item);
    } else {
        mp_raise("obj_append: not supported type %s", mp_type(a.type));
    }
    return a;
}

MpObj mp_get_global(MpObj globals, char *key) {
    MpObj okey = string_new(key);
    DictNode* node = dict_get_node(GET_DICT(globals), okey);
    if (node == NULL) {
        node = dict_get_node(GET_DICT(tm->builtins), okey);
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
        number_format(s, a);
        return string_new(s);
    }
    case TYPE_LIST: {
        MpObj str = string_new("");

        str = string_append_char(str, '[');
        int i, l = LIST_LEN(a);
        for (i = 0; i < l; i++) {
            MpObj obj = GET_LIST(a)->nodes[i];
            /* reference to self in list */
            if (obj_equals(a, obj)) {
                str = string_append_sz(str, "[...]");
            } else if (obj.type == TYPE_STR) {
                str = string_append_char(str, '"');
                str = string_append_obj(str, obj);
                str = string_append_char(str, '"');
            } else {
                str = string_append_obj(str, obj);
            }
            if (i != l - 1) {
                str = string_append_char(str, ',');
            }
        }
        str = string_append_char(str, ']');
        return str;
    }
    case TYPE_DICT:
        sprintf(buf, "<dict at %p>", GET_DICT(a));
        return string_new(buf);
    case TYPE_FUNCTION:
        FUNCTION_FORMAT(buf, a);
        return string_new(buf);
    case TYPE_CLASS:
        class_format(buf, a);
        return string_new(buf);
    case TYPE_NONE:
        return string_from_sz("None");
    case TYPE_DATA:
        return GET_DATA(a)->str(GET_DATA(a));
    default:
        mp_raise("str: not supported type %d", a.type);
    }
    return string_alloc("", 0);
}

/** get const id, this will be used to search the const value */
int get_const_id(MpObj const_value) {
    int i = dict_set(tm->constants, const_value, NONE_OBJECT);
    return i;
}


MpObj mp_call_builtin(BuiltinFunc func, int n, ...) {
    int i = 0;
    va_list ap;
    va_start(ap, n);

    arg_start();
    for (i = 0; i < n; i++) {
        arg_push(va_arg(ap, MpObj));
    }
    return func();
}