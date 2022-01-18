/**
 * HashSet
 * @author xupingmao <578749341@qq.com>
 * @since 2021/09/30 20:45:28
 * @modified 2022/01/16 15:02:06
 */
#include "include/mp.h"

/* TODO: set.contains */
MpObj dict_set_builtin_add() {
    MpObj self = arg_take_dict_obj("set.add");
    MpObj key  = arg_take_obj("set.add");
    obj_set(self, key, tm->_TRUE);
    return NONE_OBJECT;
}

MpObj dict_set_builtin_remove() {
    MpObj self = arg_take_dict_obj("set.remove");
    MpObj key  = arg_take_obj("set.remove");
    obj_del(self, key);
    return NONE_OBJECT;
}


void dict_set_methods_init() {
    MpObj set_class = class_new_by_cstr("set");
    /* build dict class */
    reg_method_by_cstr(set_class, "add", dict_set_builtin_add);
    reg_method_by_cstr(set_class, "remove", dict_set_builtin_remove);

    obj_set_by_cstr(tm->builtins, "set", set_class);
}