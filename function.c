#include "include/tm.h"


CodeCheckResult resolveCode(TmModule *mod, unsigned char*s, int isFuncDef) {
	CodeCheckResult st;
	int len = 0;
	int stacksize = 1;
	int curstack = 0;
	int temp = 0;
	int defCount = 0;
	int maxlocals = -1;
	int maxstack = 0;
	st.code = s;
	if (isFuncDef) {
		defCount = 1;
	}
	while (1) {
		int ins = next_byte(s);
		int val = next_short(s);
		len += 3;
        if (ins == NEW_STRING || ins == NEW_NUMBER) {
            len += val;
            s += val;
        } else if (ins == LOAD_LOCAL) {
			maxlocals = max(val, maxlocals);
		} else if(ins == STORE_LOCAL) {
			maxlocals = max(val, maxlocals);
		} else if(ins == TM_EOF){
			if (isFuncDef) {
				defCount--;
				if (defCount == 0)
					goto ret;
			}
		} else if(ins == TM_EOP) {
			mod->resolved = 1;
			goto ret;
		} else if(ins == TM_DEF) {
			if (isFuncDef) {
				defCount++;
			}
		}
	}
	ret: 
    st.len = len;
	if (isFuncDef) {
		st.maxlocals = maxlocals + 1; // maxlocals is max localindex in fact
	} else {
		st.maxlocals = 0;
	}
	st.maxstack = maxstack;
	return st;
}


Object newFunction(Object mod,
		Object self,
		Object (*native_func)()){
  TmFunction* f= tmMalloc(sizeof(TmFunction));
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
  Object nfnc = newFunction(fnc->mod, self, fnc->native);
  GET_FUNCTION(nfnc)->name = GET_FUNCTION(_fnc)->name;
  GET_FUNCTION(nfnc)->maxlocals = GET_FUNCTION(_fnc)->maxlocals;
  GET_FUNCTION(nfnc)->code = GET_FUNCTION(_fnc)->code;
  return nfnc;
}

Object classNew(Object clazz){
  TmDict* cl = GET_DICT(clazz);
  Object k,v;
  Object instance = newDict();
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
  tmFree(func, sizeof(TmFunction));
#if DEBUG_GC
int _new = tm->allocated_mem;
  printf("free function ,%d => %d , free %d B\n", old, _new, old - _new );
#endif
}

Object moduleNew(Object file , Object name, Object code){
  TmModule *mod = tmMalloc(sizeof(TmModule));
  mod->file = file;
  mod->code = code;
  mod->resolved = 0;
  /*mod->constants = newList(20);*/
  /*_listAppend(GET_LIST(mod->constants), NONE_OBJECT);*/
  mod->globals = newDict();
  Object m = gcTrack(newObj(TYPE_MODULE, mod));
  /* set module */
  tmSet(tm->modules, file, mod->globals);
  dictSetByStr(mod->globals, "__name__", name);
  return m;
}

void moduleFree(TmModule* mod){
  tmFree(mod, sizeof(TmModule));
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

void resolveModule(TmModule* mod, TmFunction* fnc){
	if (! mod->resolved) {
		CodeCheckResult st = resolveCode(mod, (unsigned char*) GET_STR(mod->code), 0);
		fnc->code = st.code;
		fnc->maxlocals = st.maxlocals;
		fnc->maxstack = st.maxstack;
	}
}
unsigned char* getFunctionCode(TmFunction *fnc){
	resolveModule(GET_MODULE(fnc->mod), fnc);
	return fnc->code;
}

/*Object* getFunctionConstants(TmFunction* fnc) {
	return LIST_NODES(GET_MODULE(fnc->mod)->constants);
}*/

Object getFunctionGlobals(TmFunction* fnc) {
	return GET_MODULE(fnc->mod)->globals;
}

int getFunctionMaxLocals(TmFunction* fnc){
	resolveModule(GET_MODULE(fnc->mod), fnc);
	return fnc->maxlocals;
}

Object getModuleCreateIfNotExist(char* modName) {
	Object mod = newString0(modName, -1);
	if(bTmHas(tm->modules, mod)) {
		return tmGet(tm->modules, mod);
	}else {
		tmSet(tm->modules, mod, newDict());
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
	resolveModule(GET_MODULE(fnc->mod), fnc);
	return GET_MODULE(fnc->mod);
}
#endif