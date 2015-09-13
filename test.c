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
	dictSetByStr(tm->modules, "boot", newDict());
	dictSetByStr(tm->builtins, "tm", newNumber(1));
	dictSetByStr(tm->builtins, "True", newNumber(1));
	dictSetByStr(tm->builtins, "False", newNumber(0));
	dictSetByStr(tm->builtins, "__builtins__", tm->builtins);
	dictSetByStr(tm->builtins, "__modules__", tm->modules);
	STRING_MAIN = newString0("__main__", -1); 
	regConst(STRING_MAIN);
	STRING_NAME = newString0("__name__", -1);
	regConst(STRING_NAME);

	regListMethods();
	regStringMethods();
	regDictMethods();
	regBuiltinsFunc();
    regBuiltinsFunc2();
}


void loadModule(char* fname, unsigned char* s) {
	Object modName = newString0(fname, strlen(fname));
	Object code = newString0((char*) s, -1);
	Object mod = moduleNew(modName, modName, code);
	Object fnc = newFunction(mod, NONE_OBJECT, NULL);
	GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
	GET_FUNCTION(fnc)->name = STRING_MAIN;
	callFunction(fnc);
}

void loadModule2(Object name, Object code) {
	Object mod = moduleNew(name, name, code);
	Object fnc = newFunction(mod, NONE_OBJECT, NULL);
	GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
	GET_FUNCTION(fnc)->name = STRING_MAIN;
	callFunction(fnc);
}

int callModFunc(char* mod, char* szFnc) {
    Object m = tmGet(tm->modules, newString0(mod, strlen(mod)));
    Object fnc = tmGet(m, newString0(szFnc, strlen(szFnc)));
    argStart();
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
    i = newNumber(0);
    max = newNumber(1000000);
    n34 = newNumber(34);
    n1 = newNumber(1);
    while (tm_bool(tmLessThan(i, max))) {
        // j = tmMul(i, n34);
        if (IS_NUM(i) && IS_NUM(n34)) {
            Object temp = newNumber(GET_NUM(i) * GET_NUM(n34));
            j = temp;
        }
        i = tmAdd(i, n1);
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
    Object s0 = staticString("abcdefg");
    Object s1 = staticString("cd");
    Object s2 = staticString("==");
    Object s3 = subString(s0.value.str, 0, 2);

    argStart();
    pushArg(s0);
    pushArg(s1);
    pushArg(s2);
    Object s4 = bmStringReplace();
    tmPrintf("'abcdefg'.substring(0,2)=%o\n", s3);
    tmPrintf("'abcdefg'.replace('cd','==')=%o\n", s4);
}

void testDict() {
	Object d = newDict();
	Object v = newString0("test", -1);
	Object k = newString0("name", -1);
	Object k2 = newNumber(12);
	Object k3 = newString0("age",-1);
	Object k4 = newString0("just", -1);
	Object k5 = newString0("just2", -1);
	tmSet(d,k,v);
	tmSet(d,k2,v);
	tmSet(d,k3,v);
	tmSet(d,k4,v);
	DictNode* node = DictGetNode(GET_DICT(d), k5);
	// printf("node = %s\n", node);
	DictPrint(GET_DICT(d));
	tmPrintln(d);
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
        gcInit();
        // testRun(argc, argv);
        testString();
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    gcFree();
    free(tm);
	return 0;
}

int main(int argc, char* argv[]) {
	return testInit(argc, argv);
}
