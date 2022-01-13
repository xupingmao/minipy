/**
 * HashSet
 * @author xupingmao <578749341@qq.com>
 * @since 2021/09/30 20:45:28
 * @modified 2021/09/30 22:24:31
 */
#include "include/mp.h"

/* TODO: set.contains */
MpObj dictset_builtin_add() {
    MpObj self = arg_take_dict_obj("set.add");
    MpObj key  = arg_take_obj("set.add");
    obj_set(self, key, tm->_TRUE);
    return NONE_OBJECT;
}

MpObj dictset_builtin_remove() {
    MpObj self = arg_take_dict_obj("set.remove");
    MpObj key  = arg_take_obj("set.remove");
    obj_del(self, key);
    return NONE_OBJECT;
}


void dictset_methods_init() {
    MpObj set_class = class_new_by_cstr("set");
    /* build dict class */
    reg_method_by_cstr(set_class, "add", dictset_builtin_add);
    reg_method_by_cstr(set_class, "remove", dictset_builtin_remove);

    obj_set_by_cstr(tm->builtins, "set", set_class);
}