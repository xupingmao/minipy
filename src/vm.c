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
#include "tmarg.c"
#include "module/time.c"

/** do not need to boot from binary **/
#ifndef TM_NO_BIN
    #include "bin.c"
#endif
    
void reg_mod_func(Object mod, char* name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = sz_to_string(name);
    obj_set(mod,GET_FUNCTION(func)->name, func);
}

void reg_builtin_func(char* name, Object (*native)()) {
    reg_mod_func(tm->builtins, name, native);
}

void load_module(Object name, Object code) {
    Object mod = module_new(name, name, code);
    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = sz_to_string("#main");
    call_function(fnc);
}

int call_mod_func(char* mod, char* sz_fnc) {
    Object m = obj_get(tm->modules, string_new(mod));
    Object fnc = obj_get(m, string_new(sz_fnc));
    arg_start();
    call_function(fnc);
    return 0;
}

int load_binary() {
#ifdef TM_NO_BIN
    return 0;
#else
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
#endif
}

int vm_init(int argc, char* argv[]) {
    
    int i;
    
    tm = malloc(sizeof(TmVm));
    if (tm == NULL) {
        fprintf(stderr, "vm init fail");
        return -1;
    }

    // init gc
    gc_init();

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
    time_mod_init();
    
    Object p = list_new(argc);
    for (i = 1; i < argc; i++) {
        Object arg = string_new(argv[i]);
        obj_append(p, arg);
    }
    dict_set_by_str(tm->builtins, "ARGV", p);
    return 0;
}

void vm_destroy() {
    gc_destroy();
    free(tm);
}

int tm_init(int argc, char* argv[]) {
    int ret = vm_init(argc, argv);
    if (ret != 0) { 
        return ret;
    }
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        load_binary();
        call_mod_func("init", "boot");
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    vm_destroy();
    return 0;
}

/** 
 * run c function
 * @param mod_name mocked module name
 */
int tm_run_func(int argc, char* argv[], char* mod_name, void(*func)(void)) {

    int ret = vm_init(argc, argv);
    tm->local_obj_list = untracked_list_new(100);
    if (ret != 0) { 
        return ret;
    }
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        
        // 
        Object *_argv = dict_get_by_str(tm->builtins, "ARGV");
        if (_argv == NULL) {
            tm_raise("ARGV is not defined!");
        }
        Object argv = *_argv;
        list_insert(GET_LIST(argv), 0, string_new(mod_name));

        load_binary();

        tm->gc_state = 0;
        func();
        tm->gc_state = 1;
        printf("tm->max_allocated = %d\n", tm->max_allocated);
        fgetc(stdin);

    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    vm_destroy();
    return 0;
}
