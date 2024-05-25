/**
 * HashSet
 * @author xupingmao <578749341@qq.com>
 * @since 2021/09/30 20:45:28
 * @modified 2022/04/12 21:13:38
 */
#include "include/mp.h"
#include "include/dict.h"

static MpDict* DictSet_GetItems(MpDict* dictSet) {
    assert(dictSet != NULL);
    MpObj *items = dict_get_by_cstr(dictSet, "__items");
    if (items == NULL) {
        mp_raise("set: __items is empty");
        return NULL;
    } else {
        if (IS_DICT(*items)) {
            return GET_DICT(*items);
        } else {
            mp_raise("set: __items is not dict");
        }
    }
    return NULL;
}

/* TODO: set.contains */
static MpObj DictSet_add() {
    MpDict* self = mp_take_dict_ptr_arg("set.add");
    MpObj key  = mp_take_obj_arg("set.add");
    MpDict *items = DictSet_GetItems(self);
    assert(items != NULL);
    dict_set0(items, key, tm->_TRUE);
    return NONE_OBJECT;
}

static MpObj DictSet_remove() {
    MpDict* self = mp_take_dict_ptr_arg("set.remove");
    MpObj key  = mp_take_obj_arg("set.remove");
    MpDict* items = DictSet_GetItems(self);
    assert (items != NULL);
    dict_pop(items, key, &NONE_OBJECT);
    return NONE_OBJECT;
}

static MpObj DictSet_init() {
    MpDict* self = mp_take_dict_ptr_arg("set.init");
    MpObj items = dict_new();

    dict_set_by_cstr(self, "__items", items);

    if (mp_count_arg() == 1) {
        return NONE_OBJECT;
    }
    MpList *list = mp_take_list_ptr_arg("set.init");
    for (int i = 0; i < list->len; i++) {
        obj_set(items, list_get(list, i), tm->_TRUE);
    }
    return NONE_OBJECT;
}

void DictSet_InitMethods() {
    // TODO 目前实现有问题
    MpObj set_class = class_new_by_cstr("set");
    /* build dict class */
    mp_reg_method(set_class, "add", DictSet_add);
    mp_reg_method(set_class, "remove", DictSet_remove);
    mp_reg_method(set_class, "__init__", DictSet_init);

    obj_set_by_cstr(tm->builtins, "set", set_class);
}