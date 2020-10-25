#include "../include/mp.h"


void sys_mod_argv_init(MpObj sys_mod) {
    MpObj p = list_new(tm->argc);
    int i;
    for (i = 1; i < tm->argc; i++) {
        MpObj arg = string_new(tm->argv[i]);
        obj_append(p, arg);
    }
    dict_set_by_str(sys_mod, "argv", p);
}

void sys_mod_modules_init(MpObj sys_mod) {
    dict_set_by_str(sys_mod, "modules", tm->modules);
    dict_set_by_str(sys_mod, "version", string_new("minipy"));
}

void sys_mod_init() {
    MpObj sys_mod  = dict_new();
    MpObj sys_path = list_new(2);
    sys_mod_argv_init(sys_mod);
    sys_mod_modules_init(sys_mod);
    dict_set_by_str(sys_mod, "path", sys_path);

    // register to modules
    dict_set_by_str(tm->modules, "sys",  sys_mod);
}

