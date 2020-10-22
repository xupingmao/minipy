/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2020/10/23 00:42:47
 */

#include "include/mp.h"
#include "string.c"
#include "list.c"
#include "number.c"
#include "gc.c"
#include "builtins.c"
#include "ops.c"
#include "dict.c"
#include "function.c"
#include "exception.c"
#include "tmarg.c"
#include "execute.c"
#include "module/time.c"
#include "module/sys.c"
#include "module/math.c"
#include "module/os.c"

/**
 * register module function
 * @param mod, module object, dict
 */
void reg_mod_func(Object mod, char* name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_from_sz(name);
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
    GET_FUNCTION(fnc)->name = string_from_sz("#main");
    call_function(fnc);
}

/**
 * @since 2016-11-20
 */
Object load_file_module(Object file, Object code, Object name) {
    Object mod = module_new(file, name, code);
    // resolve cache
    mp_resolve_code(GET_MODULE(mod), GET_STR(code));

    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = string_new("#main");
    GET_FUNCTION(fnc)->cache = GET_MODULE(mod)->cache;
    call_function(fnc);
    return GET_MODULE(mod)->globals;
}

/**
 * @since 2016-11-27
 */
Object load_boot_module(char* sz_filename, char* sz_code) {
    Object name = string_new(sz_filename);
    Object file = name;
    Object code = string_new("");
    Object mod  = module_new(file, name, code);

    mp_resolve_code(GET_MODULE(mod), sz_code);
    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = string_new("#main");
    GET_FUNCTION(fnc)->cache = GET_MODULE(mod)->cache;
    
    call_function(fnc);
    return GET_MODULE(mod)->globals;
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
 * init virtual machine
 * built-in objects
 * built-in functions
 * built-in modules
 */
int vm_init(int argc, char* argv[]) {
    
    int i;

    #ifdef TM_CHECK_MEM
        ptr_map = PtrMap_new();
    #endif

    tm = malloc(sizeof(TmVm));
    if (tm == NULL) {
        fprintf(stderr, "vm init fail");
        return -1;
    }

    // vm init done at gc_init()
    // init gc
    gc_init();

    tm->argc   = argc;
    tm->argv   = argv;
    tm->code   = NULL;
    tm->steps  = 0;
    tm->_TRUE  = tm_number(1);
    tm->_FALSE = tm_number(0);

    /* set module boot */
    Object boot = dict_new();
    obj_set(tm->modules, string_from_sz("boot"), boot);
    obj_set(boot, string_from_sz("__name__"), string_from_sz("boot"));

    /* builtins constants */
    dict_set_by_str(tm->builtins, "tm",    tm_number(1));
    dict_set_by_str(tm->builtins, "True",  tm_number(1));
    dict_set_by_str(tm->builtins, "False", tm_number(0));
    dict_set_by_str(tm->builtins, "__builtins__", tm->builtins);
    dict_set_by_str(tm->builtins, "__modules__",  tm->modules);
    
    list_methods_init();
    string_methods_init();
    dict_methods_init();
    builtins_init();

    /* init c modules */
    time_mod_init();
    sys_mod_init();
    math_mod_init();
    os_mod_init();
    
    return 0;
}

/**
 * destroy virtual machine
 * destroy goes with init
 */
void vm_destroy() {
#ifdef TM_PRINT_STEPS
    printf("steps = %d\n", tm->steps);
#endif
    gc_destroy();
    free(tm);

#ifdef TM_CHECK_MEM
    PtrMap_free(ptr_map);
#endif
}
