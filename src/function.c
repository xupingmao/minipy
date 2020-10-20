/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2020/10/21 01:13:41
 */
#include "include/mp.h"

/**
 * check function's byte code
 * @return the ending pointer of the func
 */
unsigned char* func_resolve(TmFunction* fnc, unsigned char* pc) {
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
        } else if(op == OP_EOF){
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
TmCodeCache* func_resolve_cache(TmFunction* fnc, TmCodeCache* cache) {
    int maxlocals = -1;
    int maxstack = 0;
    int defs = 0;
    TmCodeCache* cache_head = cache;
    
    if (fnc->resolved) {
        return fnc->cache_end;
    }
    while (1) {
        int op = cache->op;
        int val = cache->v.ival;
        if (op == OP_LOAD_LOCAL || op == OP_STORE_LOCAL) {
            maxlocals = max(cache->v.ival, maxlocals);
        } else if (op == OP_EOF) {
            defs--;
            if (defs == 0) {
                break;
            }
        } else if (op == OP_EOP) {
            break;
        } else if (op == OP_DEF) {
            defs++;
        } else if (op == 0) {
            tm_raise("func_resolve_cache: op=%s", op);
        }
        cache++;
    }

    fnc->resolved = 1;
    fnc->maxlocals = maxlocals + 1;
    fnc->cache = cache_head + 1;
    fnc->cache_end = cache + 1;
    return fnc->cache_end;
}

Object func_new(Object mod,
        Object self,
        Object (*native_func)()){
  tm_assert_type2(mod, TYPE_MODULE, TYPE_NONE, "func_new");

  TmFunction* f= tm_malloc(sizeof(TmFunction));
  f->resolved = 0;
  f->mod = mod;
  // f->code = code;
  f->code = NULL;
  f->native = native_func;
  f->maxlocals = 0;
  f->self = self;
  f->name = NONE_OBJECT;
  return gc_track(obj_new(TYPE_FUNCTION, f));
}

Object method_new(Object _fnc, Object self){
  tm_assert_type(_fnc, TYPE_FUNCTION, "method_new");
  TmFunction* fnc = GET_FUNCTION(_fnc);
  Object nfnc = func_new(fnc->mod, self, fnc->native);
  GET_FUNCTION(nfnc)->name = GET_FUNCTION(_fnc)->name;
  GET_FUNCTION(nfnc)->maxlocals = GET_FUNCTION(_fnc)->maxlocals;
  GET_FUNCTION(nfnc)->code = GET_FUNCTION(_fnc)->code;
  GET_FUNCTION(nfnc)->cache = GET_FUNCTION(_fnc)->cache;
  return nfnc;
}

Object class_new(Object name) {
  // TODO add class type
  MpClass* clazz = tm_malloc(sizeof(MpClass));
  clazz->name = name;
  clazz->attr_dict = dict_new();
  return gc_track(obj_new(TYPE_CLASS, clazz));
}


Object class_instance(Object clazz){
  MpClass *pclass = GET_CLASS(clazz);
  TmDict* cl = GET_DICT(pclass->attr_dict);
  Object k,v;
  Object instance = dict_new();
  DictNode* nodes = cl->nodes;
  int i;
  for(i = 0; i < cl->cap; i++) {
      k = nodes[i].key;
      v = nodes[i].val;
      if(nodes[i].used && IS_FUNC(v)){
        Object method = method_new(v, instance);
        obj_set(instance, k, method);
      }
  }
  return instance;
}

void class_free(MpClass* pclass) {
  tm_free(pclass, sizeof(MpClass));
}

void class_format(char* dest, Object class_obj) {
  tm_assert_type(class_obj, TYPE_CLASS, "class_format");
  MpClass* clazz = GET_CLASS(class_obj);
  sprintf(dest, "<class %s@%p>", GET_SZ(clazz->name), clazz);
}


void func_free(TmFunction* func){
  // the references will be tracked by gc collecter
  tm_free(func, sizeof(TmFunction));
}

/**
 * @param file filename
 * @name  __name__
 */
Object module_new(Object file, Object name, Object code){
  TmModule *mod = tm_malloc(sizeof(TmModule));
  mod->file = file;
  mod->code = code;
  mod->resolved = 0;
  mod->cache = NULL;
  /*mod->constants = list_new(20);*/
  /*list_append(GET_LIST(mod->constants), NONE_OBJECT);*/
  mod->globals = dict_new();
  Object m = gc_track(obj_new(TYPE_MODULE, mod));
  /* set module */
  obj_set(tm->modules, file, mod->globals);
  dict_set_by_str(mod->globals, "__name__", name);
  return m;
}

/**
 * @since 2016-11-25
 */
void tm_init_cache(TmModule* m) {
    if (m->cache != NULL) return;
    m->cache_cap = 100;
    m->cache_len = 0;
    m->cache = tm_malloc(sizeof(TmCodeCache) * m->cache_cap);
}

/**
 * @since 2016-11-25
 */
void tm_push_cache(TmModule* m, TmCodeCache cache) {
    if (m->cache_len>=m->cache_cap) {
        int oldsize = m->cache_cap * sizeof(TmCodeCache);
        m->cache_cap = m->cache_cap + m->cache_cap / 2 + 1;
        int newsize = m->cache_cap * sizeof(TmCodeCache);
        m->cache = tm_realloc(m->cache, oldsize, newsize);
    }
    // printf("cache: %3d - %s\n", cache.op, cache.sval);
    m->cache[m->cache_len] = cache;
    m->cache_len++;
}

void module_free(TmModule* mod){
    if (mod->cache != NULL) {
        tm_free(mod->cache, sizeof(TmCodeCache) * mod->cache_cap);
    }
    tm_free(mod, sizeof(TmModule));
}


void func_format(char* des, TmFunction* func){
    char sz_buf[20];
    char* sz_fnc = GET_STR(func->name);
    strncpy(sz_buf, sz_fnc, 19);
    if (func->self.type != TYPE_NONE) {
        sprintf(des, "<method %p %s>", func, sz_buf);
    } else if (func->native != NULL) {
        sprintf(des, "<function (native) %p %s>", func, sz_buf);
    } else {
        sprintf(des, "<function %p %s>", func, sz_buf);
    }
}

Object func_get_code_obj(TmFunction* func) {
    if (func->native != NULL) {
        return NONE_OBJECT;
    }
    unsigned char* code = func->code;
    int len = 0;
    while (code[len] != OP_EOF) {
        len += 3;
    }
    len += 3; /* OP_EOF */
    return string_alloc((char*)code, len);
}


Object func_get_attr(TmFunction* fnc, Object key) {
    if(obj_eq_sz(key, "__name__")) {
        return fnc->name;
    }else if(obj_eq_sz(key, "__self__")) {
        return fnc->self;
    } else if (obj_eq_sz(key, "__code__")) {
        return func_get_code_obj(fnc);
    }
    return NONE_OBJECT;
}

unsigned char* func_get_code(TmFunction *fnc){
    //resolve_module(GET_MODULE(fnc->mod), fnc);
    return fnc->code;
}

Object get_function_globals(TmFunction* fnc) {
    tm_assert_type(fnc->mod, TYPE_MODULE, "get_function_globals");
    return GET_MODULE(fnc->mod)->globals;
}

int get_function_max_locals(TmFunction* fnc){
    //resolve_module(GET_MODULE(fnc->mod), fnc);
    return fnc->maxlocals;
}

char* get_func_name_sz(Object func) {
    if (IS_FUNC(func)) {
        return GET_STR(GET_FUNCTION(func)->name);
    } else if (IS_CLASS(func)){
        return "<class>";
    } else {
        return "<unknown>";
    }
}

/**
 * @param func Object
 * @return file_name Obj->string
 */
Object get_file_name_obj(Object func) {
  if (IS_FUNC(func)) {
    return GET_MODULE(GET_FUNCTION(func)->mod)->file;
  }
  return NONE_OBJECT;
}

Object get_func_name_obj(Object func) {
  if (IS_FUNC(func)) {
    return GET_FUNCTION(func)->name;
  }
  return NONE_OBJECT;
}
