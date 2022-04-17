/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/04/12 21:09:51
 */
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
        } else if(op == OP_DEF_END){
            defs--;
            if (defs == 0) break;
        } else if(op == OP_EOP) {
            break;
        } else if(op == OP_DEF) {
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
    
    if (fnc->resolved) {
        return fnc->cache_end;
    }
    while (1) {
        int op = cache->op;
        int val = cache->v.ival;
        if (op == OP_LOAD_LOCAL || op == OP_STORE_LOCAL) {
            maxlocals = max(cache->v.ival, maxlocals);
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

    fnc->resolved = 1;
    fnc->maxlocals = maxlocals + 1;
    fnc->cache = cache_head + 1;
    fnc->cache_end = cache + 1;
    return fnc->cache_end;
}

MpObj func_new(MpObj mod,
               MpObj self,
               MpNativeFunc native_func){
    // module可以为空或者Module类型
    mp_assert_type2(mod, TYPE_MODULE, TYPE_NONE, "func_new");

    MpFunction* f= mp_malloc(sizeof(MpFunction));
    f->resolved = 0;
    f->mod = mod;
    f->code = NULL;
    f->native = native_func;
    f->maxlocals = 0;
    f->self = self;
    f->name = NONE_OBJECT;
    return gc_track(obj_new(TYPE_FUNCTION, f));
}

/**
 * create new method from function
 */
MpObj method_new(MpObj _fnc, MpObj self){
    mp_assert_type(_fnc, TYPE_FUNCTION, "method_new");
    MpFunction* fnc = GET_FUNCTION(_fnc);
    MpObj nfnc = func_new(fnc->mod, self, fnc->native);
    GET_FUNCTION(nfnc)->name = GET_FUNCTION(_fnc)->name;
    GET_FUNCTION(nfnc)->maxlocals = GET_FUNCTION(_fnc)->maxlocals;
    GET_FUNCTION(nfnc)->code = GET_FUNCTION(_fnc)->code;
    GET_FUNCTION(nfnc)->cache = GET_FUNCTION(_fnc)->cache;
    return nfnc;
}

MpObj class_new(MpObj name) {
    // TODO add class type
    MpClass* clazz = mp_malloc(sizeof(MpClass));
    clazz->name = name;
    clazz->attr_dict = dict_new();
    return gc_track(obj_new(TYPE_CLASS, clazz));
}

MpObj class_new_by_cstr(char* name) {
    MpObj name_obj = string_new(name);
    return class_new(name_obj);
}


MpObj class_instance(MpObj clazz){
    MpClass *pclass = GET_CLASS(clazz);
    MpDict* cl = GET_DICT(pclass->attr_dict);
    MpObj k,v;
    MpObj instance = dict_new();
    DictNode* nodes = cl->nodes;
    int i;
    for(i = 0; i < cl->cap; i++) {
        k = nodes[i].key;
        v = nodes[i].val;
        if(nodes[i].used && IS_FUNC(v)){
          MpObj method = method_new(v, instance);
          obj_set(instance, k, method);
        }
    }
    
    MpObj *_fnc = dict_get_by_cstr(GET_DICT(instance), "__init__");
    if (_fnc != NULL) {
        obj_call(*_fnc);
    }

    return instance;
}

void class_free(MpClass* pclass) {
   mp_free(pclass, sizeof(MpClass));
}

void class_format(char* dest, MpObj class_obj) {
    mp_assert_type(class_obj, TYPE_CLASS, "class_format");
    MpClass* clazz = GET_CLASS(class_obj);
    sprintf(dest, "<class %s@%p>", GET_CSTR(clazz->name), clazz);
}


void func_free(MpFunction* func){
    // the references will be tracked by gc collecter
    mp_free(func, sizeof(MpFunction));
}

/**
 * @param file filename
 * @name  __name__
 */
MpObj module_new(MpObj fname, MpObj name, MpObj code){
  MpModule *mod = mp_malloc(sizeof(MpModule));
  mod->file = fname;
  mod->code = code;
  mod->resolved = 0;
  mod->cache = NULL;
  /*mod->constants = list_new(20);*/
  /*list_append(GET_LIST(mod->constants), NONE_OBJECT);*/
  mod->globals = dict_new();
  MpObj m = gc_track(obj_new(TYPE_MODULE, mod));
  /* set module */
  obj_set(tm->modules, fname, mod->globals);
  obj_set(mod->globals, string_static("__name__"), name);
  return m;
}

/**
 * @since 2016-11-25
 */
void mp_init_cache(MpModule* m) {
    if (m->cache != NULL) return;
    m->cache_cap = 100;
    m->cache_len = 0;
    m->cache = mp_malloc(sizeof(MpCodeCache) * m->cache_cap);
}

/**
 * @since 2016-11-25
 */
void mp_push_cache(MpModule* m, MpCodeCache cache) {
    if (m->cache_len>=m->cache_cap) {
        int oldsize = m->cache_cap * sizeof(MpCodeCache);
        m->cache_cap = m->cache_cap + m->cache_cap / 2 + 1;
        int newsize = m->cache_cap * sizeof(MpCodeCache);
        m->cache = mp_realloc(m->cache, oldsize, newsize);
    }
    // printf("cache: %3d - %s\n", cache.op, cache.sval);
    m->cache[m->cache_len] = cache;
    m->cache_len++;
}

void module_free(MpModule* mod){
    if (mod->cache != NULL) {
        mp_free(mod->cache, sizeof(MpCodeCache) * mod->cache_cap);
    }
    mp_free(mod, sizeof(MpModule));
}


void func_format(char* des, MpFunction* func){
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
    if(obj_eq_cstr(key, "__name__")) {
        return fnc->name;
    } else if(obj_eq_cstr(key, "__self__")) {
        return fnc->self;
    } else if (obj_eq_cstr(key, "__code__")) {
        return func_get_code_obj(fnc);
    }
    return NONE_OBJECT;
}

unsigned char* func_get_code(MpFunction *fnc){
    //resolve_module(GET_MODULE(fnc->mod), fnc);
    return fnc->code;
}

MpObj func_get_globals(MpFunction* fnc) {
    if (MP_TYPE(fnc->mod) != TYPE_MODULE) {
        mp_raise(
          "func_get_globals: expect module but see type:%d (func:%o) func_addr:%p", 
          MP_TYPE(fnc->mod), 
          fnc->name,
          fnc);
    }
    return GET_MODULE(fnc->mod)->globals;
}

int func_get_max_locals(MpFunction* fnc){
    //resolve_module(GET_MODULE(fnc->mod), fnc);
    return fnc->maxlocals;
}

char* func_get_name_cstr(MpObj func) {
    if (IS_FUNC(func)) {
        return GET_CSTR(GET_FUNCTION(func)->name);
    } else if (IS_CLASS(func)){
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


MpObj obj_call(MpObj func) {
    MpObj ret = NONE_OBJECT;
    if (IS_FUNC(func)) {
        RESOLVE_METHOD_SELF(func);
        mp_log_call(func);
        
        /* call native function */
        if (GET_FUNCTION(func)->native != NULL) {
            return GET_FUNCTION(func)->native();
        } else {
            MpFrame* f = push_frame(func);

            L_recall:
            if (setjmp(f->buf)==0) {
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
                    pop_frame();
                    longjmp(tm->frame->buf, 1);
                }
            }
        }
    } else if (IS_CLASS(func)) {
        return class_instance(func);
    }
    mp_raise("File %o, line=%d: obj_call:invalid object %o", 
        GET_FUNCTION_FILE(tm->frame->fnc), 
        tm->frame->lineno, 
        func);
    return NONE_OBJECT;
}


MpObj obj_apply(MpObj func, MpObj args) {
    mp_assert_type2(func, TYPE_FUNCTION, TYPE_CLASS, "obj_apply");
    mp_assert_type(args, TYPE_LIST, "obj_apply");
    arg_set_arguments(LIST_NODES(args), LIST_LEN(args));
    return obj_call(func);
}

MpObj obj_call_nargs(MpObj func, int n, MpObj* args) {
    arg_set_arguments(args, n);
    return obj_call(func);
}
