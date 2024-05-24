#include "../include/mp.h"


static void init_argv(MpObj sys_mod) {
    MpObj p = list_new(tm->argc);
    int i;
    for (i = 1; i < tm->argc; i++) {
        MpObj arg = string_new(tm->argv[i]);
        obj_append(p, arg);
    }
    obj_set_by_cstr(sys_mod, "argv", p);
}

static MpObj sys_exit() {
    int code = arg_take_int("sys.exit");
    exit(code);
    return NONE_OBJECT;
}

static void init_version(MpObj sys_mod) {
    obj_set_by_cstr(sys_mod, "modules", tm->modules);
    obj_set_by_cstr(sys_mod, "version", string_new("minipy"));
}

void mp_sys_init() {
    MpObj sys_mod  = mp_new_native_module("sys");
    MpObj sys_path = list_new(2);
    init_argv(sys_mod);
    init_version(sys_mod);
    
    obj_set_by_cstr(sys_mod, "path", sys_path);
    obj_set_by_cstr(sys_mod, "executable", string_new(tm->argv[0]));
    mod_reg_func(sys_mod, "exit", sys_exit);
}

