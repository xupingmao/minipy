/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/01/17 23:52:02
 */

#include "include/mp.h"
#include "string.c"
#include "list.c"
#include "number.c"
#include "gc.c"
#include "builtins.c"
#include "obj_ops.c"
#include "dict.c"
#include "dict_set.c"
#include "function.c"
#include "exception.c"
#include "argument.c"
#include "execute.c"
#include "module/time.c"
#include "module/sys.c"
#include "module/math.c"
#include "module/os.c"
#include "module/mp_debug.c"
#include "module/mp_random.c"


/**
 * register module
 */
void reg_mod(char* name, MpObj mod) {
    mp_assert_type(mod, TYPE_DICT, "reg_module");
    obj_set(tm->modules, string_new(name), mod);
}

/**
 * register module function
 * @param mod, module object, dict
 */
void reg_mod_func(MpObj mod, char* name, MpObj (*native)()) {
    MpObj func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_from_cstr(name);
    obj_set(mod,GET_FUNCTION(func)->name, func);
}

// 注册模块的变量
void reg_mod_attr(MpObj module, char* name, MpObj value) {
    obj_set(module, string_from_cstr(name), value);
}

/**
 * register built-in function
 */
void reg_builtin_func(char* name, MpObj (*native)()) {
    reg_mod_func(tm->builtins, name, native);
}

void reg_method_by_cstr(MpObj class_obj, char* name, MpObj (*native)()) {
    mp_assert_type(class_obj, TYPE_CLASS, "reg_method_by_cstr");
    MpClass* clazz = GET_CLASS(class_obj);
    MpObj attr_dict = clazz->attr_dict;

    MpObj func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_new(name);
    
    obj_set_by_cstr(attr_dict, name, func);
}

/**
 * load module
 */
void load_module(MpObj name, MpObj code) {
    MpObj mod = module_new(name, name, code);
    MpObj fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_CSTR(code);
    GET_FUNCTION(fnc)->name = string_from_cstr("#main");
    obj_call(fnc);
}

/**
 * @since 2016-11-20
 */
MpObj load_file_module(MpObj file, MpObj code, MpObj name) {
    MpObj mod = module_new(file, name, code);
    // resolve cache
    mp_resolve_code(GET_MODULE(mod), GET_CSTR(code));

    MpObj fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_CSTR(code);
    GET_FUNCTION(fnc)->name = string_new("#main");
    GET_FUNCTION(fnc)->cache = GET_MODULE(mod)->cache;
    obj_call(fnc);
    return GET_MODULE(mod)->globals;
}

/**
 * @since 2016-11-27
 */
MpObj load_boot_module(char* sz_filename, char* sz_code) {
    MpObj name = string_new(sz_filename);
    MpObj file = name;
    MpObj code = string_new("");
    MpObj mod  = module_new(file, name, code);
    
    mp_resolve_code(GET_MODULE(mod), sz_code);
    MpObj fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_CSTR(code);
    GET_FUNCTION(fnc)->name = string_new("#main");
    GET_FUNCTION(fnc)->cache = GET_MODULE(mod)->cache;
    
    obj_call(fnc);
    
    return GET_MODULE(mod)->globals;
}

/**
 * call function of module
 * @param mod, module name
 * @param sz_fnc, function name
 */
MpObj vm_call_mod_func(char* mod, char* sz_fnc) {
    MpObj module = obj_get(tm->modules, string_new(mod));
    MpObj fnc = obj_get(module, string_new(sz_fnc));
    arg_start();
    return obj_call(fnc);
}

/**
 * init virtual machine
 * built-in objects
 * built-in functions
 * built-in modules
 */
int vm_init(int argc, char* argv[]) {
    
    int i;

    #ifdef MP_CHECK_MEM
        ptr_map = PtrMap_new();
    #endif

    tm = malloc(sizeof(MpVm));
    if (tm == NULL) {
        fprintf(stderr, "vm init fail");
        return -1;
    }

    log_init();
    
    // vm init done at gc_init()
    // init gc
    gc_init();

    // 初始化Profile
    PROFILE_INIT();

    tm->argc   = argc;
    tm->argv   = argv;
    tm->code   = NULL;
    tm->steps  = 0;
    tm->_TRUE  = number_obj(1);
    tm->_FALSE = number_obj(0);

    /* set module boot */
    MpObj boot = dict_new();
    obj_set(boot, string_from_cstr("__name__"), string_from_cstr("boot"));

    reg_mod("boot", boot);

    /* builtins constants */
    obj_set_by_cstr(tm->builtins, "tm",    number_obj(1));
    obj_set_by_cstr(tm->builtins, "True",  number_obj(1));
    obj_set_by_cstr(tm->builtins, "False", number_obj(0));
    obj_set_by_cstr(tm->builtins, "__builtins__", tm->builtins);
    obj_set_by_cstr(tm->builtins, "__modules__",  tm->modules);
    
    list_methods_init();
    string_methods_init();
    dict_methods_init();
    dict_set_methods_init();
    builtins_init();

    /* init c modules */
    time_mod_init();
    sys_mod_init();
    math_mod_init();
    os_mod_init();
    init_mod_for_random();
    init_mod_for_debug();
    
    return 0;
}

/**
 * destroy virtual machine
 * destroy goes with init
 */
void vm_destroy() {
#ifdef MP_PRINT_STEPS
    printf("steps = %d\n", tm->steps);
#endif
    gc_destroy();
    free(tm);

    log_destroy();

#ifdef MP_CHECK_MEM
    PtrMap_free(ptr_map);
#endif

#if MP_PROFILE
    profile_print_detail();
#endif
}
