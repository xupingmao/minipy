#include "include/tm.h"
#include "include/vm.h"
#include "include/exception.h"
#include "lib/StringBuilder.c"
#include "string.c"
#include "list.c"
#include "number.c"
#include "gc.c"
#include "core.c"
#include "builtins.c"
#include "builtins2.c"
#include "ops.c"
#include "dict.c"
#include "function.c"
#include "interp.c"
#include "exception.c"
#include "code.c"
#include "tmdata.c"
#include "tmarg.c"
#include "bin.c"
#include <time.h>
void regConst(Object constant) {
    tm_append(tm->root, constant);
}

void regModFunc(Object mod, char* name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_alloc(name, -1);
    tm_set(mod,GET_FUNCTION(func)->name, func);
}

void regBuiltinFunc(char* name, Object (*native)()) {
    regModFunc(tm->builtins, name, native);
}

void builtinsInit() {
    /* set module boot */
    dictSetByStr(tm->modules, "boot", dict_new());
    dictSetByStr(tm->builtins, "tm", tm_number(1));
    dictSetByStr(tm->builtins, "True", tm_number(1));
    dictSetByStr(tm->builtins, "False", tm_number(0));
    dictSetByStr(tm->builtins, "__builtins__", tm->builtins);
    dictSetByStr(tm->builtins, "__modules__", tm->modules);
    STRING_MAIN = string_alloc("__main__", -1); 
    regConst(STRING_MAIN);
    STRING_NAME = string_alloc("__name__", -1);
    regConst(STRING_NAME);

    list_methods_init();
    string_methods_init();
    dict_methods_init();
    builtin_funcs_init();
    builtin_funcs_init2();
}


void loadModule(char* fname, unsigned char* s) {
    Object modName = string_alloc(fname, strlen(fname));
    Object code = string_alloc((char*) s, -1);
    Object mod = module_new(modName, modName, code);
    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = STRING_MAIN;
    callFunction(fnc);
}

void loadModule2(Object name, Object code) {
    Object mod = module_new(name, name, code);
    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = STRING_MAIN;
    callFunction(fnc);
}

int callModFunc(char* mod, char* szFnc) {
    Object m = tm_get(tm->modules, string_alloc(mod, strlen(mod)));
    Object fnc = tm_get(m, string_alloc(szFnc, strlen(szFnc)));
    arg_start();
    callFunction(fnc);
    return 0;
}


long test_clock() {
#ifdef TM_NT
    return (clock());
#else
    return (clock()/1000);
#endif
}

void testRun(int argc, char* argv[]) {
    Object i, j, n1, n34, max;
    long t1, t2;
    t1 = test_clock();
    i = tm_number(0);
    max = tm_number(1000000);
    n34 = tm_number(34);
    n1 = tm_number(1);
    while (tm_bool(tmLessThan(i, max))) {
        // j = tm_mul(i, n34);
        if (IS_NUM(i) && IS_NUM(n34)) {
            Object temp = tm_number(GET_NUM(i) * GET_NUM(n34));
            j = temp;
        }
        i = tm_add(i, n1);
    }
    t2 = test_clock();
    printf("t1: time=%ld\n", t2-t1);
    
    double ci = 0, cj;
    t1 = test_clock();
    while (ci < 1000000) {
        cj = ci * 34;
        ci = ci + 1;
    }
    t2 = test_clock();
    printf("t2: time=%ld\n", t2-t1);
}


void testString() {
    Object s0 = string_static("abcdefg");
    Object s1 = string_static("cd");
    Object s2 = string_static("==");
    Object s3 = string_substring(s0.value.str, 0, 2);

    arg_start();
    arg_push(s0);
    arg_push(s1);
    arg_push(s2);
    Object s4 = string_m_replace();
    tm_printf("'abcdefg'.substring(0,2)=%o\n", s3);
    tm_printf("'abcdefg'.replace('cd','==')=%o\n", s4);
}

void testDict() {
    Object d = dict_new();
    Object v = string_alloc("test", -1);
    Object k = string_alloc("name", -1);
    Object k2 = tm_number(12);
    Object k3 = string_alloc("age",-1);
    Object k4 = string_alloc("just", -1);
    Object k5 = string_alloc("just2", -1);
    tm_set(d,k,v);
    tm_set(d,k2,v);
    tm_set(d,k3,v);
    tm_set(d,k4,v);
    DictNode* node = dict_get_node(GET_DICT(d), k5);
    // printf("node = %s\n", node);
    DictPrint(GET_DICT(d));
    tm_println(d);
}

int testInit(int argc, char* argv[]) {
    tm = malloc(sizeof(TmVM));
    if (tm == NULL) {
        fprintf(stderr, "vm init fail");
        return -1;
    }
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        gc_init();
        // testRun(argc, argv);
        testString();
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    gc_free();
    free(tm);
    return 0;
}

int main(int argc, char* argv[]) {
    return testInit(argc, argv);
}
