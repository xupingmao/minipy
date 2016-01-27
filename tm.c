#include "include/tm.h"
#include "include/vm.h"
#include "include/exception.h"
#include "StringBuilder.c"
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

void regModFunc(Object mod, char* name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_static(name);
    tm_set(mod,GET_FUNCTION(func)->name, func);
}

void regBuiltinFunc(char* name, Object (*native)()) {
    regModFunc(tm->builtins, name, native);
}

void builtinsInit() {
    /* set module boot */
    Object boot = dict_new();
    dictSetByStr(tm->modules, "boot", boot);
    dictSetByStr(boot, "__name__", string_static("boot"));
    dictSetByStr(tm->builtins, "tm", tm_number(1));
    dictSetByStr(tm->builtins, "True", tm_number(1));
    dictSetByStr(tm->builtins, "False", tm_number(0));
    dictSetByStr(tm->builtins, "__builtins__", tm->builtins);
    dictSetByStr(tm->builtins, "__modules__", tm->modules);
    
    list_methods_init();
    string_methods_init();
    dict_methods_init();
    builtin_funcs_init();
}

void loadModule(Object name, Object code) {
    Object mod = module_new(name, name, code);
    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = string_static("#main");
    callFunction(fnc);
}

int callModFunc(char* mod, char* szFnc) {
    Object m = tm_get(tm->modules, string_new(mod));
    Object fnc = tm_get(m, string_new(szFnc));
    arg_start();
    callFunction(fnc);
    return 0;
}

int run_py_func(int argc, char* argv[], void(*func)(void)) {
    tm = malloc(sizeof(TmVM));
    if (tm == NULL) {
        fprintf(stderr, "vm init fail");
        return -1;
    }
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        gc_init();
        Object p = list_new(argc);
        int i;
        for (i = 1; i < argc; i++) {
            Object arg = string_new(argv[i]);
            tm_append(p, arg);
        }
        builtinsInit();
        dictSetByStr(tm->builtins, "ARGV", p);
        func();
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    gc_free();
    free(tm);
    return 0;
}