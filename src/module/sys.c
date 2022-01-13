#include "../include/mp.h"


void sys_mod_argv_init(MpObj sys_mod) {
    MpObj p = list_new(tm->argc);
    int i;
    for (i = 1; i < tm->argc; i++) {
        MpObj arg = string_new(tm->argv[i]);
        obj_append(p, arg);
    }
    obj_set_by_cstr(sys_mod, "argv", p);
}

MpObj sys_exit() {
    int code = arg_take_int("sys.exit");
    exit(code);
    return NONE_OBJECT;
}

void sys_mod_modules_init(MpObj sys_mod) {
    obj_set_by_cstr(sys_mod, "modules", tm->modules);
    obj_set_by_cstr(sys_mod, "version", string_new("minipy"));
}

void sys_mod_init() {
    MpObj sys_mod  = dict_new();
    MpObj sys_path = list_new(2);
    sys_mod_argv_init(sys_mod);
    sys_mod_modules_init(sys_mod);
    obj_set_by_cstr(sys_mod, "path", sys_path);
    reg_mod_func(sys_mod, "exit", sys_exit);

    // register to modules
    reg_mod("sys", sys_mod);
}

