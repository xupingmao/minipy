/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2023-12-07 22:03:29
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-06-02 16:05:22
 * @FilePath: /minipy/src/vm.c
 * @Description: 描述
 */
/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/06/10 22:54:27
 */

#include "include/mp.h"
#include "string.c"
#include "list.c"
#include "number.c"
#include "gc.c"
#include "builtins.c"
#include "object_ops.c"
#include "dict.c"
#include "dict_set.c"
#include "function.c"
#include "klass.c"
#include "exception.c"
#include "argument.c"
#include "execute.c"
#include "import.c"
#include "code_object.c"
#include "constant_pool.c"
#include "object.c"
#include "module.c"

#include "module/mp_time.c"
#include "module/mp_sys.c"
#include "module/mp_math.c"
#include "module/mp_os.c"
#include "module/mp_debug.c"
#include "module/mp_random.c"
#include "module/mp_file.c"


#include "gen/mp_init.gen.c"
#include "gen/mp_tokenize.gen.c"
#include "gen/mp_parse.gen.c"
#include "gen/mp_encode.gen.c"
#include "gen/mp_opcode.gen.c"
#include "gen/pyeval.gen.c"
#include "gen/repl.gen.c"

/**
 * register built-in function
 */
void mp_reg_builtin_func(char* name, MpObj (*native)()) {
    MpModule_RegFunc(tm->builtins, name, native);
}

void mp_reg_method(MpObj class_obj, char* name, MpObj (*native)()) {
    mp_assert_type(class_obj, TYPE_CLASS, "mp_reg_method");
    MpClass* clazz = GET_CLASS(class_obj);
    MpDict* attr_dict = clazz->attr_dict;
    MpObj func = func_new(tm->builtins_mod, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_new(name);
    
    dict_set_by_cstr(attr_dict, name, func);
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
    GET_FUNCTION(fnc)->resolved = 1;
    MP_CALL_EX(fnc);
    return GET_MODULE(mod)->globals;
}

/**
 * @since 2016-11-27
 */
MpObj load_boot_module(char* sz_filename, const char* sz_code) {
    MpObj name = string_new(sz_filename);
    MpObj file = name;
    MpObj code = string_new("");
    MpObj mod  = module_new(file, name, code);
    
    mp_resolve_code(GET_MODULE(mod), sz_code);
    MpObj fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_CSTR(code);
    GET_FUNCTION(fnc)->name = string_new("#main");
    GET_FUNCTION(fnc)->cache = GET_MODULE(mod)->cache;
    GET_FUNCTION(fnc)->resolved = 1;
    
    MP_CALL_EX(fnc);
    
    return GET_MODULE(mod)->globals;
}

/**
 * call function of module
 * @param mod, module name
 * @param sz_fnc, function name
 */
MpObj vm_call_mod_func(const char* mod, const char* sz_fnc) {
    MpObj module = mp_getattr(tm->modules, string_static(mod));
    MpObj fnc = mp_getattr(module, string_static(sz_fnc));
    #ifdef MP_DEBUG
        mp_printf("vm_call_mod_func.function: %o\n", fnc);
        mp_printf("vm_call_mod_func.module: %o\n", func_get_mod_obj(GET_FUNCTION(fnc)));
    #endif
    mp_reset_args();
    return MP_CALL_EX(fnc);
}

/**
 * init virtual machine
 * built-in objects
 * built-in functions
 * built-in modules
 */
int vm_init(int argc, char* argv[]) {
    int i;

    /* 使用静态内存，尽量避免动态内存分配 */
    static MpVm _vm;

    #ifdef MP_CHECK_MEM
        ptr_map = PtrMap_new();
    #endif

    tm = &_vm;

    log_init();
    
    // vm init done at gc_init()
    // init gc
    gc_init();

    // 初始化Profile
    PROFILE_INIT();

    tm->mp2c_mode = FALSE;
    tm->argc   = argc;
    tm->argv   = argv;
    tm->code   = NULL;
    tm->steps  = 0;
    tm->_TRUE  = mp_number(1);
    tm->_FALSE = mp_number(0);
    tm->vm_size = sizeof(MpVm);

    /* set module boot */
    MpObj boot = mp_new_native_module("boot");

    /* builtins constants */
    obj_set_by_cstr(tm->builtins, "tm",    mp_number(1));
    obj_set_by_cstr(tm->builtins, "True",  mp_number(1));
    obj_set_by_cstr(tm->builtins, "False", mp_number(0));
    obj_set_by_cstr(tm->builtins, "__builtins__", tm->builtins);
    obj_set_by_cstr(tm->builtins, "__modules__",  tm->modules);
    
    MpList_InitMethods();
    MpStr_InitMethods();
    MpDict_InitMethods();
    DictSet_InitMethods();
    mp_init_builtins();

    /* init c modules */
    mp_time_init();
    mp_sys_init();
    mp_math_init();
    mp_os_init();
    mp_file_init();
    mp_random_init();
    mp_debug_init();
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

    log_destroy();

#ifdef MP_CHECK_MEM
    PtrMap_free(ptr_map);
#endif

#if MP_PROFILE
    profile_print_detail();
#endif
}


void vm_load_py_modules() {
    /* load python modules */
    load_boot_module("mp_init",     mp_init_bin);
    load_boot_module("mp_opcode",   mp_opcode_bin);
    load_boot_module("mp_tokenize", mp_tokenize_bin);
    load_boot_module("mp_parse",    mp_parse_bin);
    load_boot_module("mp_encode",   mp_encode_bin);
    load_boot_module("pyeval",      pyeval_bin);
    load_boot_module("repl",        repl_bin);
    
    log_debug("vm_load_py_modules done!\n");
}