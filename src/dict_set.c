/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2024-05-25 20:50:31
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-06-01 23:48:34
 * @FilePath: /minipy/src/dict_set.c
 * @Description: collection structure
 */
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

static MpObj DictSet_add() {
    MpInstance* self = mp_take_instance_arg("set.add");
    MpObj key  = mp_take_obj_arg("set.add");
    MpDict *items = DictSet_GetItems(self->dict);
    assert(items != NULL);
    dict_set0(items, key, tm->_TRUE);
    return NONE_OBJECT;
}

static MpObj DictSet_remove() {
    MpInstance* self = mp_take_instance_arg("set.remove");
    MpObj key  = mp_take_obj_arg("set.remove");
    MpDict* items = DictSet_GetItems(self->dict);
    assert (items != NULL);
    dict_pop(items, key, &NONE_OBJECT);
    return NONE_OBJECT;
}

static MpObj DictSet_contains() {
    MpInstance* self = mp_take_instance_arg("set.contains");
    MpObj value = mp_take_obj_arg("set.contains");
    MpDict* items = DictSet_GetItems(self->dict);
    assert (items != NULL); 
    DictNode* result = dict_get_node(items, value);
    if (result == NULL) {
        return tm->_FALSE;
    } else {
        return tm->_TRUE;
    }
}

static MpObj DictSet_len() {
    MpInstance* self = mp_take_instance_arg("set.len");
    MpDict* items = DictSet_GetItems(self->dict);
    assert (items != NULL); 
    return mp_number(items->len);
}

static MpObj DictSet_init() {
    MpInstance* self = mp_take_instance_arg("set.init");
    MpObj items = dict_new();

    dict_set_by_cstr(self->dict, "__items", items);

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
    MpClass* set_class = MpClass_New("set", tm->builtins_mod);
    /* build dict class */
    MpClass_RegNativeMethod(set_class, "add", DictSet_add);
    MpClass_RegNativeMethod(set_class, "remove", DictSet_remove);
    MpClass_RegNativeMethod(set_class, "__contains__", DictSet_contains);
    MpClass_RegNativeMethod(set_class, "__init__", DictSet_init);
    MpClass_RegNativeMethod(set_class, "__len__", DictSet_len);
    dict_set_by_cstr(tm->builtins, "set", MpClass_ToObj(set_class));
}