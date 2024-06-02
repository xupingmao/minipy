#include "../include/mp.h"


static void init_argv(MpObj sys_mod) {
    MpObj p = list_new(tm->argc);
    int i;
    for (i = 1; i < tm->argc; i++) {
        MpObj arg = string_new(tm->argv[i]);
        mp_append(p, arg);
    }
    obj_set_by_cstr(sys_mod, "argv", p);
}

static MpObj sys_exit() {
    int code = mp_take_int_arg("sys.exit");
    exit(code);
    return NONE_OBJECT;
}

static void init_version(MpObj sys_mod) {
    obj_set_by_cstr(sys_mod, "modules", tm->modules);
    obj_set_by_cstr(sys_mod, "version", string_new("minipy"));
}

static MpObj sys_getsizeof() {
    MpObj obj = mp_take_obj_arg("sys.getsizeof");
    size_t size = mp_sizeof(obj);
    if (size == 0) {
        mp_raise("type(%s) not implemented", get_object_type_cstr(obj));
    }
    return mp_number(size);
}

void mp_sys_init() {
    MpObj sys_mod  = mp_new_native_module("sys");
    MpObj sys_path = list_new(2);
    init_argv(sys_mod);
    init_version(sys_mod);
    
    obj_set_by_cstr(sys_mod, "path", sys_path);
    obj_set_by_cstr(sys_mod, "executable", string_new(tm->argv[0]));
    obj_set_by_cstr(sys_mod, "modules", tm->modules);
    
    MpModule_RegFunc(sys_mod, "exit", sys_exit);
    MpModule_RegFunc(sys_mod, "getsizeof", sys_getsizeof);
}

