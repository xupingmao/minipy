/**
 * HashSet
 * @author xupingmao <578749341@qq.com>
 * @since 2021/09/30 20:45:28
 * @modified 2022/04/12 21:13:38
 */
#include "include/mp.h"

/* TODO: set.contains */
static MpObj dict_set_builtin_add() {
    MpObj self = arg_take_dict_obj("set.add");
    MpObj key  = arg_take_obj("set.add");
    obj_set(self, key, tm->_TRUE);
    return NONE_OBJECT;
}

static MpObj dict_set_builtin_remove() {
    MpObj self = arg_take_dict_obj("set.remove");
    MpObj key  = arg_take_obj("set.remove");
    obj_del(self, key);
    return NONE_OBJECT;
}

static MpObj dict_set_builtin_init() {
    MpObj self = arg_take_dict_obj("set.init");
    if (arg_count() == 0) {
        return NONE_OBJECT;
    }
    MpList *list = arg_take_list_ptr("set.init");
    for (int i = 0; i < list->len; i++) {
        obj_set(self, list_get(list, i), tm->_TRUE);
    }
    return NONE_OBJECT;
}

void dict_set_methods_init() {
    MpObj set_class = class_new_by_cstr("set");
    /* build dict class */
    reg_method_by_cstr(set_class, "add", dict_set_builtin_add);
    reg_method_by_cstr(set_class, "remove", dict_set_builtin_remove);
    reg_method_by_cstr(set_class, "__init__", dict_set_builtin_init);

    obj_set_by_cstr(tm->builtins, "set", set_class);
}