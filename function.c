#include "include/tm.h"

unsigned char* resolve_func(TmFunction* fnc, unsigned char* pc) {
    int maxlocals = -1;
    int maxstack = 0;
    int defs = 0;
    unsigned char* s = pc;
    if (fnc->resolved) {
        return fnc->end;
    }
    while (1) {
		int op = next_byte(s);
		int val = next_short(s);
        if (op == NEW_STRING || op == NEW_NUMBER) {
            s += val;
        } else if (op == LOAD_LOCAL || op == STORE_LOCAL) {
			maxlocals = max(val, maxlocals);
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

Object func_new(Object mod,
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
  return gcTrack(newObj(TYPE_FUNCTION, f));
}

Object methodNew(Object _fnc, Object self){
  TmFunction* fnc = GET_FUNCTION(_fnc);
  Object nfnc = func_new(fnc->mod, self, fnc->native);
  GET_FUNCTION(nfnc)->name = GET_FUNCTION(_fnc)->name;
  GET_FUNCTION(nfnc)->maxlocals = GET_FUNCTION(_fnc)->maxlocals;
  GET_FUNCTION(nfnc)->code = GET_FUNCTION(_fnc)->code;
  return nfnc;
}

Object classNew(Object clazz){
  TmDict* cl = GET_DICT(clazz);
  Object k,v;
  Object instance = dict_new();
  DictNode* nodes = cl->nodes;
  int i;
  for(i = 0; i < cl->cap; i++) {
      k = nodes[i].key;
      v = nodes[i].val;
      if(nodes[i].used && IS_FUNC(v)){
        Object method = methodNew(v, instance);
        tmSet(instance, k, method);
      }
  }
  return instance;
}

void functionFree(TmFunction* func){
  // the references will be tracked by gc collecter
#if DEBUG_GC
 printf("free function %p...\n", func);
 int old = tm->allocated_mem;
#endif
  tm_free(func, sizeof(TmFunction));
#if DEBUG_GC
int _new = tm->allocated_mem;
  printf("free function ,%d => %d , free %d B\n", old, _new, old - _new );
#endif
}

Object moduleNew(Object file , Object name, Object code){
  TmModule *mod = tm_malloc(sizeof(TmModule));
  mod->file = file;
  mod->code = code;
  mod->resolved = 0;
  /*mod->constants = list_new(20);*/
  /*_listAppend(GET_LIST(mod->constants), NONE_OBJECT);*/
  mod->globals = dict_new();
  Object m = gcTrack(newObj(TYPE_MODULE, mod));
  /* set module */
  tmSet(tm->modules, file, mod->globals);
  dictSetByStr(mod->globals, "__name__", name);
  return m;
}

void moduleFree(TmModule* mod){
  tm_free(mod, sizeof(TmModule));
}


void _functionFormat(char* des, TmFunction* func){
	char szBuf[20];
    char* szFnc = GET_STR(func->name);
    strncpy(szBuf, szFnc, 19);
	if (func->self.type != TYPE_NONE) {
		sprintf(des, "<method %p %s>", func, szBuf);
	} else {
		sprintf(des, "<function %p %s>", func, szBuf);
	}
}

Object getFuncAttr(TmFunction* fnc, Object key) {
	if(objEqSz(key, "func_name")) {
		return fnc->name;
	}else if(objEqSz(key, "__self__")) {
		return fnc->self;
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

Object getModuleCreateIfNotExist(char* modName) {
	Object mod = string_alloc(modName, -1);
	if(tm_has(tm->modules, mod)) {
		return tmGet(tm->modules, mod);
	}else {
		tmSet(tm->modules, mod, dict_new());
		return tmGet(tm->modules, mod);
	}
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
#if DB_FUNC
char* getFuncFileSz(Object func) {
    if (IS_FUNC(func)) {
        return GET_STR(GET_MODULE(GET_FUNCTION(func)->mod)->file);
    } else if (IS_DICT(func)){
        return "<class>";
    } else {
        return "<unknown>";
    }
}

TmModule* getFuncMod(TmFunction* fnc) {
	// resolveModule(GET_MODULE(fnc->mod), fnc);
	return GET_MODULE(fnc->mod);
}
#endif