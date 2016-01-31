#include "include/tm.h"
#include "include/vm.h"
#include "include/exception.h"
#include "string.c"
#include "list.c"
#include "number.c"
#include "gc.c"
#include "builtins.c"
#include "ops.c"
#include "dict.c"
#include "function.c"
#include "interp.c"
#include "exception.c"
#include "util.c"
#include "tmarg.c"
#include "bin.c"

void regModFunc(Object mod, char* name, Object (*native)()) {
    Object func = funcNew(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = szToString(name);
    objSet(mod,GET_FUNCTION(func)->name, func);
}

void regBuiltinFunc(char* name, Object (*native)()) {
    regModFunc(tm->builtins, name, native);
}

void vmInit() {
    /* set module boot */
    Object boot = dictNew();
    dictSetByStr(tm->modules, "boot", boot);
    dictSetByStr(boot, "__name__", szToString("boot"));
    dictSetByStr(tm->builtins, "tm", tmNumber(1));
    dictSetByStr(tm->builtins, "True", tmNumber(1));
    dictSetByStr(tm->builtins, "False", tmNumber(0));
    dictSetByStr(tm->builtins, "__builtins__", tm->builtins);
    dictSetByStr(tm->builtins, "__modules__", tm->modules);
    
    listMethodsInit();
    stringMethodsInit();
    dictMethodsInit();
    builtinsInit();
}

void loadModule(Object name, Object code) {
    Object mod = moduleNew(name, name, code);
    Object fnc = funcNew(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = szToString("#main");
    callFunction(fnc);
}

int callModFunc(char* mod, char* szFnc) {
    Object m = objGet(tm->modules, stringNew(mod));
    Object fnc = objGet(m, stringNew(szFnc));
    arg_start();
    callFunction(fnc);
    return 0;
}

int loadBinary() {
    unsigned char* text = bin;
    int count = uncode32(&text);
    int i;for(i = 0; i < count; i++) {
        int nameLen = uncode32(&text);
        Object name = stringAlloc((char*)text, nameLen);
        text += nameLen;
        int codeLen = uncode32(&text);
        Object code = stringAlloc((char*)text, codeLen);
        text += codeLen;
        loadModule(name, code);
    }
    return 1;
}

int tmRun(int argc, char* argv[]) {
    Object p = listNew(argc);
    int i;
    for (i = 1; i < argc; i++) {
        Object arg = stringNew(argv[i]);
        objAppend(p, arg);
    }
    vmInit();
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
    gcDeinit();
    free(tm);
    return 0;
}

int main(int argc, char* argv[]) {
    return tmInit(argc, argv);
}
