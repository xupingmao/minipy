#include "../include/tm.h"


void sys_mod_argv_init(Object sys_mod) {
    Object p = list_new(tm->argc);
    int i;
    for (i = 1; i < tm->argc; i++) {
        Object arg = string_new(tm->argv[i]);
        obj_append(p, arg);
    }
    dict_set_by_str(sys_mod, "argv", p);
}

void sys_mod_modules_init(Object sys_mod) {
    dict_set_by_str(sys_mod, "modules", tm->modules);
}

void sys_mod_init() {
    Object sys_mod = dict_new();
    sys_mod_argv_init(sys_mod);
    sys_mod_modules_init(sys_mod);
    dict_set_by_str(tm->modules, "sys", sys_mod);
}

