#include "include/tm.h"

#define RET_NUM(v) \
    Object _num;\
    TM_TYPE(_num) = TYPE_NUM;\
    GET_NUM(_num) = v;\
    return _num;

void tmAssertType(Object o, int type, char* msg) {
    if (TM_TYPE(o) != type) {
        tmRaise("%s, expect %s but see %s", msg, 
            getTypeByInt(type), getTypeByObj(o));
    }
}

void tmAssertInt(double value, char* msg) {
    if (value != (int) value) {
        tmRaise("%s require int", msg);
    }
}

int objEqSz(Object obj, const char* value) {
	return TM_TYPE(obj) == TYPE_STR
			&& (GET_STR(obj) == value || strcmp(GET_STR(obj), value) == 0);
}

void tmSet(Object self, Object k, Object v) {
	switch (TM_TYPE(self)) {
	case TYPE_LIST: {
		tmAssertType(k, TYPE_NUM, "tmSet");
		double d = GET_NUM(k);
		tmAssertInt(d, "listSet");
		ListSet(GET_LIST(self), (int)d, v);
	}
		return;
	case TYPE_DICT:
		DictSet(GET_DICT(self), k, v);
		return;
	}
	tmRaise("tmSet: Self %o, Key %o, Val %o", self, k, v);
}

Object tmGet(Object self, Object k) {
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
				tmRaise("StringGet: index overflow ,len=%d,index=%d, str=%o",
						GET_STR_LEN(self), n, self);
			return tmChr(0xff & GET_STR(self)[n]);
		} else if ((node = DictGetNode(GET_DICT(CLASS_STRING), k)) != NULL) {
			return methodNew(node->val, self);
		}
		break;
	}
	case TYPE_LIST:{
		DictNode* node;
		if (TM_TYPE(k) == TYPE_NUM) {
			return ListGet(GET_LIST(self), GET_NUM(k));
		} else if ((node = DictGetNode(GET_DICT(CLASS_LIST), k))!=NULL) {
			return methodNew(node->val, self);
		}
		break;
	}
	case TYPE_DICT:{
		DictNode* node;
		node = DictGetNode(GET_DICT(self), k);
		if (node != NULL) {
			return node->val;
		} else if ((node = DictGetNode(GET_DICT(CLASS_DICT), k))!=NULL) {
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
	tmRaise("keyError %o", k);
	return NONE_OBJECT;
}

Object tmSub(Object a, Object b) {
	if (a.type == b.type) {
		if (a.type == TYPE_NUM) {
            GET_NUM(a) -= GET_NUM(b);
            return a;
		}
	}
	tmRaise("tmSub: can not substract %o and %o", a, b);
	return NONE_OBJECT;
}

Object tmAdd(Object a, Object b) {
	if (TM_TYPE(a) == TM_TYPE(b)) {
		switch (TM_TYPE(a)) {
		case TYPE_NUM:
			GET_NUM(a) += GET_NUM(b);
			return a;
		case TYPE_STR: {
			char* sa = GET_STR(a);
			char* sb = GET_STR(b);
			int la = GET_STR_LEN(a);
			int lb = GET_STR_LEN(b);
			if (la == 0) {return b;	}
			if (lb == 0) {return a;	}
			int len = la + lb;
			Object des = newString0(NULL, len);
			char*s = GET_STR(des);
			memcpy(s, sa, la);
			memcpy(s + la, sb, lb);
			return des;
		}
		case TYPE_LIST: {
			return listAdd(a, b);
		}
		}
	}
	tmRaise("tmAdd: can not add %o and %o", (a), (b));
	return NONE_OBJECT;
}

int tmEquals(Object a, Object b){
	if(TM_TYPE(a) != TM_TYPE(b)) return 0;
	switch(TM_TYPE(a)){
		case TYPE_NUM:return GET_NUM(a) == GET_NUM(b);
		case TYPE_STR: {
            String* s1 = GET_STR_OBJ(a);
            String* s2 = GET_STR_OBJ(b);
			return s1->value == s2->value || 
                (s1->len == s2->len && strncmp(s1->value, s2->value, s1->len) == 0);
		}
		case TYPE_LIST:	{
			if(GET_LIST(a) == GET_LIST(b)) return 1;
			int i;
			int len = GET_LIST(a)->len;
			Object* nodes1 = GET_LIST(a)->nodes;
			Object* nodes2 = GET_LIST(b)->nodes;
			for(i = 0; i < len; i++){
				if(!tmEquals(nodes1[i], nodes2[i]) ){
					return 0;
				}
			}
			return 1;
		}
		case TYPE_NONE:return 1;
		case TYPE_DICT:return GET_DICT(a) == GET_DICT(b);
		case TYPE_FUNCTION: return GET_FUNCTION(a) == GET_FUNCTION(b);
		default: tmRaise("equals(): not supported type %d", a.type);
	}
	return 0;
}

/*
#define DEF_CMP_FUNC(fnc_name, op) Object fnc_name(Object a, Object b) {      \
	if(TM_TYPE(a) != TM_TYPE(b))                             \
		tmRaise(#fnc_name"(): can not compare [%o] and [%o]", (a), (b));                 \
	switch(a.type){                      \
		case TYPE_NUM: { RET_NUM(GET_NUM(a) op GET_NUM(b) );} \
		case TYPE_STR: { RET_NUM(strcmp(GET_STR(a), GET_STR(b)) op 0); } \
		default : tmRaise(#fnc_name"() not support yet"); \
	}                           \
	return NONE_OBJECT;\
}

#define DEF_CMP_FUNC_2(fnc_name, op) int fnc_name(Object a, Object b) {      \
	if(TM_TYPE(a) != TM_TYPE(b))                             \
		tmRaise(#fnc_name"(): can not compare [%o] and [%o]", (a), (b));                 \
	switch(a.type){                      \
		case TYPE_NUM: return  GET_NUM(a) op GET_NUM(b); \
		case TYPE_STR: return  strcmp(GET_STR(a) , GET_STR(b)) op 0; \
		default : tmRaise(#fnc_name"() not support yet"); \
	}                           \
    return 0;\
}

DEF_CMP_FUNC(tmLessThan, <);
DEF_CMP_FUNC(tmGreaterThan, >);
DEF_CMP_FUNC(tmLessEqual, <=);
DEF_CMP_FUNC(tmGreaterEqual, >=);
*/

int tmCmp(Object a, Object b) {
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
    tmRaise("tmCmp: can not compare %o and %o", a, b);
    return 0;
}
/*
DEF_CMP_FUNC_2(tm_bool_lt, <);
DEF_CMP_FUNC_2(tm_bool_gt, >);
DEF_CMP_FUNC_2(tm_bool_lteq, <=);
DEF_CMP_FUNC_2(tm_bool_gteq, >=);
*/

Object tmMul(Object a, Object b) {
	if (a.type == b.type && a.type == TYPE_NUM) {
		GET_NUM(a) *= GET_NUM(b);
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
			return staticString("");
		if (times == 1)
			return a;
        des = newString0(NULL, len * times);
        char* s = GET_STR(des);
		int i;
		for (i = 0; i < times; i++) {
			strncpy(s, GET_STR(a), len);
            s += len;
		}
		return des;
	}
	tmRaise("tmMul: can not multiply %o and %o", a, b);
	return NONE_OBJECT;
}

Object tmDiv(Object a, Object b) {
	if (a.type == b.type && a.type == TYPE_NUM) {
		GET_NUM(a) /= GET_NUM(b);
        return a;
	}
	tmRaise("tmDiv: can not divide %o and %o", a, b);
	return NONE_OBJECT;
}

Object tmMod(Object a, Object b) {
	if (a.type == b.type && a.type == TYPE_NUM) {
		return newNumber((long) GET_NUM(a) % (long) GET_NUM(b));
	} else if (a.type == TYPE_STR) {
		Object *__mod__ = getBuiltin("__mod__");
		if (__mod__ == NULL) {
			tmRaise("__mod__ is not defined");
		} else {
			argStart();
			pushArg(a);
			pushArg(b);
			return callFunction(*__mod__);
		}		
	}
	tmRaise("tmMod: can not module %o and %o", a, b);
	return NONE_OBJECT;
}

/* a has b */
int tm_has(Object a, Object b) {
	switch (TM_TYPE(a)) {
	case TYPE_LIST: {
		return (_listIndex(GET_LIST(a), b) != -1);
	}
	case TYPE_STR: {
		if (TM_TYPE(b) != TYPE_STR)
			return 0;
		return StringIndex(GET_STR_OBJ(a), GET_STR_OBJ(b), 0) != -1;
	}
	case TYPE_DICT: {
		DictNode* node = DictGetNode(GET_DICT(a), b);
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
	tmRaise("tm_neg: can not handle %o", o);
	return NONE_OBJECT;
}


Object iterNew(Object collections) {
	switch(TM_TYPE(collections)) {
        case TYPE_LIST: return listIterNew(GET_LIST(collections));
        case TYPE_DICT: return dictIterNew(GET_DICT(collections));
        case TYPE_STR: return stringIterNew(GET_STR_OBJ(collections));
        case TYPE_DATA: return collections;
        default: tmRaise("iterNew(): can not iterate %o", collections);
	}
	return NONE_OBJECT;
}

Object* tm_next(Object iterator) {
    return GET_DATA_PROTO(iterator)->next(GET_DATA(iterator));
}

void tm_del(Object self, Object k) {
	switch(TM_TYPE(self)) {
		case TYPE_DICT:{
			dictDel(self, k);
			break;
		}
		case TYPE_LIST:{
			listDel(self, k);
			break;
		}
        default:
            tmRaise("tm_del: not supported type %s", getTypeByObj(self));
	}
}


