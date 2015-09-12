#include "include/tm.h"
#include "include/vm.h"
#include "include/exception.h"
#include "StringBuilder.c"
#include "string.c"
#include "list.c"
#include "number.c"
#include "gc.c"
#include "builtins.c"
#include "builtins2.c"
#include "ops.c"
#include "dict.c"
#include "function.c"
#include "interp.c"
#include "exception.c"
#include "util.c"
#include "tmdata.c"
#include "tmarg.c"
#include "bin.c"

void regConst(Object constant) {
	APPEND(tm->root, constant);
}

void regModFunc(Object mod, char* name, Object (*native)()) {
	Object func = newFunction(NONE_OBJECT, NONE_OBJECT, native);
	GET_FUNCTION(func)->name = newString0(name, -1);
	tmSet(mod,GET_FUNCTION(func)->name, func);
}

void regBuiltinFunc(char* name, Object (*native)()) {
	regModFunc(tm->builtins, name, native);
}

void builtinsInit() {
    /* set module boot */
    Object boot = newDict();
	dictSetByStr(tm->modules, "boot", boot);
    dictSetByStr(boot, "__name__", staticString("boot"));
	dictSetByStr(tm->builtins, "tm", newNumber(1));
	dictSetByStr(tm->builtins, "True", newNumber(1));
	dictSetByStr(tm->builtins, "False", newNumber(0));
	dictSetByStr(tm->builtins, "__builtins__", tm->builtins);
	dictSetByStr(tm->builtins, "__modules__", tm->modules);
    
	regListMethods();
	regStringMethods();
	regDictMethods();
	regBuiltinsFunc();
    regBuiltinsFunc2();
}

void loadModule(Object name, Object code) {
	Object mod = moduleNew(name, name, code);
	Object fnc = newFunction(mod, NONE_OBJECT, NULL);
	GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
	GET_FUNCTION(fnc)->name = staticString("#main");
	callFunction(fnc);
}

int callModFunc(char* mod, char* szFnc) {
    Object m = tmGet(tm->modules, newString(mod));
    Object fnc = tmGet(m, newString(szFnc));
    argStart();
    callFunction(fnc);
	return 0;
}

int loadBinary() {
    unsigned char* text = bin;
    int count = uncode32(&text);
    int i;for(i = 0; i < count; i++) {
        int nameLen = uncode32(&text);
        Object name = newString0((char*)text, nameLen);
        text += nameLen;
        int codeLen = uncode32(&text);
        Object code = newString0((char*)text, codeLen);
        text += codeLen;
        loadModule(name, code);
    }
	return 1;
}

int tmRun(int argc, char* argv[]) {
	Object p = newList(argc);
	int i;
	for (i = 1; i < argc; i++) {
		Object arg = newString0(argv[i], strlen(argv[i]));
		APPEND(p, arg);
	}
	builtinsInit();
	dictSetByStr(tm->builtins, "ARGV", p);
    loadBinary();
    callModFunc("init", "boot");
    return 0;
}

int tmInit(int argc, char* argv[]) {
	tm = malloc(sizeof(TmVM));
	if (tm == NULL) {
		fprintf(stderr, "vm init fail");
		return -1;
	}
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        gcInit();
        tmRun(argc, argv);
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    gcFree();
    free(tm);
	return 0;
}

int main(int argc, char* argv[]) {
	return tmInit(argc, argv);
}
