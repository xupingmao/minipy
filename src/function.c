/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2016
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-06-02 02:25:15
 * @FilePath: /minipy/src/function.c
 * @Description: minipy函数
 */
#include "include/code_cache.h"
#include "include/mp.h"

/**
 * check function's byte code
 * @return the ending pointer of the func
 */
unsigned char* func_resolve(MpFunction* fnc, unsigned char* pc) {
    int maxlocals = -1;
    int maxstack = 0;
    int defs = 0;
    unsigned char* s = pc;

    if (fnc->resolved) {
        return fnc->end;
    }
    while (1) {
        int op = READ_BYTE(s);
        int val = READ_SHORT(s);
        if (op == OP_STRING || op == OP_NUMBER) {
            s += val;
        } else if (op == OP_LOAD_LOCAL || op == OP_STORE_LOCAL) {
            maxlocals = max(val, maxlocals);
        } else if (op == OP_SETJUMP) {
            fnc->modifier = 1;
        } else if (op == OP_DEF_END) {
            defs--;
            if (defs == 0)
                break;
        } else if (op == OP_EOP) {
            break;
        } else if (op == OP_DEF) {
            defs++;
        }
    }

    fnc->resolved = 1;
    fnc->maxlocals = maxlocals + 1;
    fnc->code = pc + 3;
    fnc->end = s;
    return fnc->end;
}

/**
 * check function's byte code
 * @return the ending pointer of the func
 * @since 2016-11-27
 */
MpCodeCache* func_resolve_cache(MpFunction* fnc, MpCodeCache* cache) {
    int maxlocals = -1;
    int maxstack = 0;
    int defs = 0;
    MpCodeCache* cache_head = cache;

    if (fnc->resolved == 1) {
        return fnc->cache_end;
    }

    assert(cache->op == OP_DEF);

    while (1) {
        int op = cache->op;
        int val = get_cache_int(cache);
        if (op == OP_LOAD_LOCAL || op == OP_STORE_LOCAL) {
            maxlocals = max(val, maxlocals);
        } else if (op == OP_DEF_END) {
            defs--;
            if (defs == 0) {
                break;
            }
        } else if (op == OP_EOP) {
            break;
        } else if (op == OP_DEF) {
            defs++;
        } else if (op == 0) {
            mp_raise("func_resolve_cache: op=%s", op);
        }
        cache++;
    }

    assert(cache->op == OP_DEF_END);

    fnc->resolved = 1;
    fnc->maxlocals = maxlocals + 1;
    fnc->cache = cache_head + 1;
    fnc->cache_end = cache + 1;
    return fnc->cache_end;
}

MpObj func_new(MpObj mod, MpObj self, MpNativeFunc native_func) {
    /* module可以为空或者Module类型 */
    if (MP_TYPE(mod) != TYPE_MODULE) {
        printf("func_new: expect <module> but got %d\n", MP_TYPE(mod));
        mp_raise("func_new: expect <module>");
    }

    assert(MP_TYPE(mod) == TYPE_MODULE);

    mp_assert_type2(mod, TYPE_MODULE, TYPE_NONE, "func_new");

    MpFunction* f = mp_malloc(sizeof(MpFunction), "func.new");
    f->resolved = 0;
    f->mod = mod;
    f->code = NULL;
    f->native = native_func;
    f->maxlocals = 0;
    f->self = self;
    f->name = NONE_OBJECT;
    return gc_track(mp_to_obj(TYPE_FUNCTION, f));
}

MpFunction *mp_new_native_func(MpObj module, MpNativeFunc native_func) {
    assert(native_func != NULL);
    MpObj result = func_new(module, NONE_OBJECT, native_func);
    return GET_FUNC(result);
}

MpObj mp_new_native_func_obj(MpObj module, MpNativeFunc native_func) {
    assert(native_func != NULL);
    return func_new(module, NONE_OBJECT, native_func);
}

/**
 * create new method from function
 */
MpObj method_new(MpObj _fnc, MpObj self) {
    mp_assert_type(_fnc, TYPE_FUNCTION, "method_new");
    MpFunction* fnc = GET_FUNCTION(_fnc);
    MpObj nfnc = func_new(fnc->mod, self, fnc->native);
    GET_FUNCTION(nfnc)->name = GET_FUNCTION(_fnc)->name;
    GET_FUNCTION(nfnc)->maxlocals = GET_FUNCTION(_fnc)->maxlocals;
    GET_FUNCTION(nfnc)->code = GET_FUNCTION(_fnc)->code;
    GET_FUNCTION(nfnc)->cache = GET_FUNCTION(_fnc)->cache;
    GET_FUNCTION(nfnc)->resolved = 1;
    return nfnc;
}

void func_free(MpFunction* func) {
    // the references will be tracked by gc collecter
    mp_free(func, sizeof(MpFunction));
}

/**
 * @since 2016-11-25
 */
void mp_init_cache(MpModule* m) {
    if (m->cache != NULL)
        return;
    m->cache_cap = 100;
    m->cache_len = 0;
    m->cache = mp_malloc(sizeof(MpCodeCache) * m->cache_cap, "cache.init");
}

/**
 * @since 2016-11-25
 */
void mp_push_cache(MpModule* m, MpCodeCache cache) {
    if (m->cache_len >= m->cache_cap) {
        int oldsize = m->cache_cap * sizeof(MpCodeCache);
        m->cache_cap = m->cache_cap + m->cache_cap / 2 + 1;
        int newsize = m->cache_cap * sizeof(MpCodeCache);
        m->cache = mp_realloc(m->cache, oldsize, newsize, "cache.push");
    }
    // printf("cache: %3d - %s\n", cache.op, cache.sval);
    m->cache[m->cache_len] = cache;
    m->cache_len++;
}

void func_format(char* des, MpFunction* func) {
    char sz_buf[20];
    char* sz_fnc;

    if (NOT_STR(func->name)) {
        sz_fnc = "None";
    } else {
        sz_fnc = GET_CSTR(func->name);
    }

    strncpy(sz_buf, sz_fnc, 19);
    if (func->self.type != TYPE_NONE) {
        sprintf(des, "<method %p %s>", func, sz_buf);
    } else if (func->native != NULL) {
        sprintf(des, "<function (native) %p %s>", func, sz_buf);
    } else {
        sprintf(des, "<function %p %s>", func, sz_buf);
    }
}

MpObj func_get_code_obj(MpFunction* func) {
    if (func->native != NULL) {
        return NONE_OBJECT;
    }
    unsigned char* code = func->code;
    int len = 0;
    while (code[len] != OP_DEF_END) {
        len += 3;
    }
    len += 3; /* OP_DEF_END */
    return string_alloc((char*)code, len);
}

MpObj func_get_attr(MpFunction* fnc, MpObj key) {
    if (obj_eq_cstr(key, "__name__")) {
        return fnc->name;
    } else if (obj_eq_cstr(key, "__self__")) {
        return fnc->self;
    } else if (obj_eq_cstr(key, "__code__")) {
        return func_get_code_obj(fnc);
    }
    return NONE_OBJECT;
}

MpObj func_get_mod_obj(MpFunction* fnc) {
    return fnc->mod;
}

unsigned char* func_get_code(MpFunction* fnc) {
    assert(fnc->resolved == 1);
    // resolve_module(GET_MODULE(fnc->mod), fnc);
    return fnc->code;
}

MpObj func_get_globals(MpFunction* fnc) {
    if (MP_TYPE(fnc->mod) != TYPE_MODULE) {
        mp_raise(
            "func_get_globals: expect module but see type:%d (func:%o) "
            "func_addr:%p",
            MP_TYPE(fnc->mod), fnc->name, fnc);
    }
    return GET_MODULE(fnc->mod)->globals;
}

int func_get_max_locals(MpFunction* fnc) {
    assert(fnc->resolved == 1);
    // resolve_module(GET_MODULE(fnc->mod), fnc);
    return fnc->maxlocals;
}

char* func_get_name_cstr(MpObj func) {
    if (IS_FUNC(func)) {
        return GET_CSTR(GET_FUNCTION(func)->name);
    } else if (IS_CLASS(func)) {
        return "<class>";
    } else {
        return "<unknown>";
    }
}

/**
 * @param func MpObj
 * @return file_name Obj->string
 */
MpObj func_get_file_name_obj(MpObj func) {
    if (IS_FUNC(func)) {
        return GET_MODULE(GET_FUNCTION(func)->mod)->file;
    }
    return NONE_OBJECT;
}

MpObj func_get_name_obj(MpObj func) {
    if (IS_FUNC(func)) {
        return GET_FUNCTION(func)->name;
    }
    return NONE_OBJECT;
}

#ifdef MP_DEBUG
MpObj mp_call_func(MpObj func, const char* source, int lineno)
#else
MpObj mp_call_func(MpObj func)
#endif
{
    MpObj ret = NONE_OBJECT;
    if (IS_FUNC(func)) {
        RESOLVE_METHOD_SELF(func);
        mp_log_call(func);

        /* call native function */
        if (GET_FUNCTION(func)->native != NULL) {
            return GET_FUNCTION(func)->native();
        } else {
#ifdef MP_DEBUG
            if (GET_FUNCTION(func)->resolved == 0) {
                mp_printf("\nERROR: function not resolved\n");
                mp_printf("module: %o\n", GET_FUNCTION(func)->mod);
                mp_printf("func: %o\nsource: %s:%d\n", func, source, lineno);
            }
#endif

            assert(GET_FUNCTION(func)->resolved == 1);
            MpFrame* f = mp_push_frame(func);

        L_recall:
            if (setjmp(f->buf) == 0) {
                return mp_eval(f);
            } else {
                f = tm->frame;
                /* handle exception in this frame */
                if (f->cache_jmp != NULL) {
                    f->cache = f->cache_jmp;
                    f->cache_jmp = NULL;
                    goto L_recall;
                } else {
                    /* there is no handler, throw to prev frame */
                    mp_push_exception(f);
                    mp_pop_frame();
                    longjmp(tm->frame->buf, 1);
                }
            }
        }
    } else if (IS_CLASS(func)) {
        MpInstance* obj_ptr = class_instance(GET_CLASS(func));
        return mp_to_obj(TYPE_INSTANTCE, obj_ptr);
    }
    mp_raise("File %o, line=%d: obj_call:invalid object %o",
             GET_FUNCTION_FILE(tm->frame->fnc), tm->frame->lineno, func);
    return NONE_OBJECT;
}

MpObj mp_call_with_args(MpObj func, MpObj args) {
    mp_assert_type2(func, TYPE_FUNCTION, TYPE_CLASS, "mp_call_with_args");
    mp_assert_type(args, TYPE_LIST, "mp_call_with_args");
    mp_set_args(LIST_NODES(args), LIST_LEN(args));
    return MP_CALL_EX(func);
}

MpObj mp_call_with_nargs(MpObj func, int n, MpObj* args) {
    mp_set_args(args, n);
    return MP_CALL_EX(func);
}

MpObj mp_call_func_safe(MpObj func, int n, MpObj* args) {
    MpArgument mp_arg = mp_save_args();
    mp_set_args(args, n);
    MpObj result = MP_CALL_EX(func);
    mp_restore_args(mp_arg);
    return result;
}