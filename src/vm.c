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
#include "module/sys.c"
#include "module/math.c"
#include "module/os.c"

// #include "tmtokenize.c"

/**
 * register module function
 * @param mod, module object, dict
 */
void reg_mod_func(Object mod, char* name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = sz_to_string(name);
    obj_set(mod,GET_FUNCTION(func)->name, func);
}

/**
 * register built-in function
 */
void reg_builtin_func(char* name, Object (*native)()) {
    reg_mod_func(tm->builtins, name, native);
}

/**
 * load module
 */
void load_module(Object name, Object code) {
    Object mod = module_new(name, name, code);
    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = sz_to_string("#main");
    call_function(fnc);
}

/**
 * call function of module
 * @param mod, module name
 * @param sz_fnc, function name
 */
Object call_mod_func(char* mod, char* sz_fnc) {
    Object m = obj_get(tm->modules, string_new(mod));
    Object fnc = obj_get(m, string_new(sz_fnc));
    arg_start();
    return call_function(fnc);
}

/**
 * load bytecode binary
 * format:
 *  [4bytes:files count]
 *   files: [4bytes:bytes length][bytes]
 */
int load_binary() {
    unsigned char* text = tm->code; /* code to initialize the vm */
    if (text == NULL) {
        /* no bootstrap code */
        return 1;
    }
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

/**
 * init virtual machine
 * built-in objects
 * built-in functions
 * built-in modules
 */
int vm_init(int argc, char* argv[]) {
    
    int i;
    
    tm = malloc(sizeof(TmVm));
    if (tm == NULL) {
        fprintf(stderr, "vm init fail");
        return -1;
    }

    // init gc
    gc_init();

    tm->argc = argc;
    tm->argv = argv;
    tm->code = NULL;

    /* set module boot */
    Object boot = dict_new();
    dict_set_by_str(tm->modules, "boot", boot);
    dict_set_by_str(boot, "__name__", sz_to_string("boot"));
    dict_set_by_str(tm->builtins, "tm", tm_number(1));
    dict_set_by_str(tm->builtins, "True", tm_number(1));
    dict_set_by_str(tm->builtins, "False", tm_number(0));
    dict_set_by_str(tm->builtins, "__builtins__", tm->builtins);
    dict_set_by_str(tm->builtins, "__modules__",  tm->modules);
    
    list_methods_init();
    string_methods_init();
    dict_methods_init();
    builtins_init();
    
    return 0;
}

/**
 * destroy virtual machine
 * destroy goes with init
 */
void vm_destroy() {
    gc_destroy();
    free(tm);
}

/**
 * run vm with specified code;
 * @since 2016-11-13
 */
int tm_run(int argc, char* argv[], unsigned char* init_code) {
    int ret = vm_init(argc, argv);
    if (ret != 0) { 
        return ret;
    }
    tm->code = init_code;

    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        /* init modules */
        time_mod_init();
        sys_mod_init();
        math_mod_init();
        os_mod_init();
    
        load_binary();
        
        if (tm_hasattr(tm->modules, "init")) {
            call_mod_func("init", "boot");
        } else if (tm_hasattr(tm->modules, "main")) {
            // adjust sys.argv
            
            call_mod_func("main", "_main");
        }
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    vm_destroy();
    return 0;
}
