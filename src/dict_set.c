/**
 * HashSet
 * @author xupingmao <578749341@qq.com>
 * @since 2021/09/30 20:45:28
 * @modified 2022/04/12 21:13:38
 */
#include "include/mp.h"

/* TODO: set.contains */
static MpObj DictSet_add() {
    MpObj self = mp_take_dict_obj_arg("set.add");
    MpObj key  = mp_take_obj_arg("set.add");
    obj_set(self, key, tm->_TRUE);
    return NONE_OBJECT;
}

static MpObj DictSet_remove() {
    MpObj self = mp_take_dict_obj_arg("set.remove");
    MpObj key  = mp_take_obj_arg("set.remove");
    obj_del(self, key);
    return NONE_OBJECT;
}

static MpObj DictSet_init() {
    MpObj self = mp_take_dict_obj_arg("set.init");
    if (mp_count_arg() == 0) {
        return NONE_OBJECT;
    }
    MpList *list = mp_take_list_ptr_arg("set.init");
    for (int i = 0; i < list->len; i++) {
        obj_set(self, list_get(list, i), tm->_TRUE);
    }
    return NONE_OBJECT;
}

void DictSet_InitMethods() {
    MpObj set_class = class_new_by_cstr("set");
    /* build dict class */
    reg_method_by_cstr(set_class, "add", DictSet_add);
    reg_method_by_cstr(set_class, "remove", DictSet_remove);
    reg_method_by_cstr(set_class, "__init__", DictSet_init);

    obj_set_by_cstr(tm->builtins, "set", set_class);
}