#include "include/tm.h"
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
#include "initbin.c";

void reg_mod_func(Object mod, char* name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = sz_to_string(name);
    obj_set(mod,GET_FUNCTION(func)->name, func);
}

void reg_builtin_func(char* name, Object (*native)()) {
    reg_mod_func(tm->builtins, name, native);
}

void vm_init() {
    /* set module boot */
    Object boot = dict_new();
    dict_set_by_str(tm->modules, "boot", boot);
    dict_set_by_str(boot, "__name__", sz_to_string("boot"));
    dict_set_by_str(tm->builtins, "tm", tm_number(1));
    dict_set_by_str(tm->builtins, "True", tm_number(1));
    dict_set_by_str(tm->builtins, "False", tm_number(0));
    dict_set_by_str(tm->builtins, "__builtins__", tm->builtins);
    dict_set_by_str(tm->builtins, "__modules__", tm->modules);
    
    list_methods_init();
    string_methods_init();
    dict_methods_init();
    builtins_init();
}

void load_module(Object name, Object code) {
    Object mod = module_new(name, name, code);
    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = sz_to_string("#main");
    call_function(fnc);
}

int load_binary() {
    unsigned char* text = bin;
    int count = uncode32(&text);
    int i;for(i = 0; i < count; i++) {
        int name_len = uncode32(&text);
        Object name = string_alloc((char*)text, name_len);
        text += name_len;
        int code_len = uncode32(&text);
        Object code = string_alloc((char*)text, code_len);
        text += code_len;
        load_module(name, code);
    }
    return 1;
}

int call_mod_func(char* mod, char* sz_fnc) {
    Object m = obj_get(tm->modules, string_new(mod));
    Object fnc = obj_get(m, string_new(sz_fnc));
    arg_start();
    call_function(fnc);
    return 0;
}

int tm_run_func(int argc, char* argv[], char* mod_name, void(*func)(void)) {
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
            obj_append(p, arg);
        }
        list_insert(GET_LIST(p), 0, string_new(mod_name));
        vm_init();
        dict_set_by_str(tm->builtins, "ARGV", p);
        load_binary();
        tm->gc_state = 0;
        func();
        tm->gc_state = 1;
        printf("tm->max_allocated = %d\n", tm->max_allocated);
    } else if (code == 1){
        DEBUG("enter traceback");
        traceback();
    } else if (code == 2){
        
    }
    gc_destroy();
    free(tm);
    return 0;
}

