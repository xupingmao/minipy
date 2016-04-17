#include "include/tm.h"

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
        if (op == NEW_STRING || op == NEW_NUMBER) {
            s += val;
        } else if (op == LOAD_LOCAL || op == STORE_LOCAL) {
            maxlocals = max(val, maxlocals);
        } else if (op == SETJUMP) {
            fnc->modifier = 1;
        } else if(op == TM_EOF){
            defs--;
            if (defs == 0) break;
        } else if(op == TM_EOP) {
            break;
        } else if(op == TM_DEF) {
            defs++;
        }
    }
    fnc->resolved = 1;
    fnc->maxlocals = maxlocals + 1;
    fnc->code = pc + 3;
    fnc->end = s;
    return fnc->end;
}

Object funcNew(Object mod,
        Object self,
        Object (*native_func)()){
  TmFunction* f= tm_malloc(sizeof(TmFunction));
  f->resolved = 0;
  f->mod = mod;
  // f->code = code;
  f->code = NULL;
  f->native = native_func;
  f->maxlocals = 0;
  f->self = self;
  f->name = NONE_OBJECT;
  return gcTrack(objNew(TYPE_FUNCTION, f));
}

Object methodNew(Object _fnc, Object self){
  TmFunction* fnc = GET_FUNCTION(_fnc);
  Object nfnc = funcNew(fnc->mod, self, fnc->native);
  GET_FUNCTION(nfnc)->name = GET_FUNCTION(_fnc)->name;
  GET_FUNCTION(nfnc)->maxlocals = GET_FUNCTION(_fnc)->maxlocals;
  GET_FUNCTION(nfnc)->code = GET_FUNCTION(_fnc)->code;
  return nfnc;
}

Object classNew(Object clazz){
  TmDict* cl = GET_DICT(clazz);
  Object k,v;
  Object instance = dictNew();
  DictNode* nodes = cl->nodes;
  int i;
  for(i = 0; i < cl->cap; i++) {
      k = nodes[i].key;
      v = nodes[i].val;
      if(nodes[i].used && IS_FUNC(v)){
        Object method = methodNew(v, instance);
        objSet(instance, k, method);
      }
  }
  return instance;
}

void funcFree(TmFunction* func){
  // the references will be tracked by gc collecter
  GC_LOG_START(func, "function");
  tm_free(func, sizeof(TmFunction));
  GC_LOG_END(func, "function");
}

Object moduleNew(Object file , Object name, Object code){
  TmModule *mod = tm_malloc(sizeof(TmModule));
  mod->file = file;
  mod->code = code;
  mod->resolved = 0;
  /*mod->constants = listNew(20);*/
  /*listAppend(GET_LIST(mod->constants), NONE_OBJECT);*/
  mod->globals = dictNew();
  Object m = gcTrack(objNew(TYPE_MODULE, mod));
  /* set module */
  objSet(tm->modules, file, mod->globals);
  dictSetByStr(mod->globals, "__name__", name);
  return m;
}

void moduleFree(TmModule* mod){
  tm_free(mod, sizeof(TmModule));
}


void func_format(char* des, TmFunction* func){
    char szBuf[20];
    char* szFnc = GET_STR(func->name);
    strncpy(szBuf, szFnc, 19);
    if (func->self.type != TYPE_NONE) {
        sprintf(des, "<method %p %s>", func, szBuf);
    } else {
        sprintf(des, "<function %p %s>", func, szBuf);
    }
}

Object funcGetCode(TmFunction* func) {
    if (func->native != NULL) {
        return NONE_OBJECT;
    }
    unsigned char* code = func->code;
    int len = 0;
    while (code[len] != TM_EOF) {
        len += 3;
    }
    len += 3; /* TM_EOF */
    return stringAlloc((char*)code, len);
}


Object getFuncAttr(TmFunction* fnc, Object key) {
    if(objEqSz(key, "__name__")) {
        return fnc->name;
    }else if(objEqSz(key, "__self__")) {
        return fnc->self;
    } else if (objEqSz(key, "__code__")) {
        return funcGetCode(fnc);
    }
    return NONE_OBJECT;
}
/*
void resolveModule(TmModule* mod, TmFunction* fnc){
    if (! mod->resolved) {
        CodeCheckResult st = resolveCode(mod, (unsigned char*) GET_STR(mod->code), 0);
        fnc->code = st.code;
        fnc->maxlocals = st.maxlocals;
        fnc->maxstack = st.maxstack;
    }
}*/

unsigned char* getFunctionCode(TmFunction *fnc){
    //resolveModule(GET_MODULE(fnc->mod), fnc);
    return fnc->code;
}

Object getFunctionGlobals(TmFunction* fnc) {
    return GET_MODULE(fnc->mod)->globals;
}

int getFunctionMaxLocals(TmFunction* fnc){
    //resolveModule(GET_MODULE(fnc->mod), fnc);
    return fnc->maxlocals;
}

char* getFuncNameSz(Object func) {
    if (IS_FUNC(func)) {
        return GET_STR(GET_FUNCTION(func)->name);
    } else if (IS_DICT(func)){
        return "<class>";
    } else {
        return "<unknown>";
    }
}

/**
 * @param func Object
 * @return fileName Obj->string
 */
Object getFileNameObj(Object func) {
  if (IS_FUNC(func)) {
    return GET_MODULE(GET_FUNCTION(func)->mod)->file;
  }
  return NONE_OBJECT;
}

Object getFuncNameObj(Object func) {
  if (IS_FUNC(func)) {
    return GET_FUNCTION(func)->name;
  }
  return NONE_OBJECT;
}