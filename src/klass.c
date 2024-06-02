#include "include/mp.h"

static MpObj instance_getattr();
static MpObj instance_setattr();

static MpObj instance_setattr() {
    MpObj self = mp_take_obj_arg("self");
    MpObj key = mp_take_obj_arg("key");
    MpObj value = mp_take_obj_arg("value");

    mp_assert_type(self, TYPE_INSTANTCE, "instance_setattr");
    MpInstance* instance = GET_INSTANCE(self);
    dict_set0(instance->dict, key, value);
    return NONE_OBJECT;
}

static MpObj instance_getattr() {
    MpObj self = mp_take_obj_arg("self");
    MpObj key = mp_take_obj_arg("key");

    mp_assert_type(self, TYPE_INSTANTCE, "instance_getattr");
    MpInstance* instance = GET_INSTANCE(self);
    
    // 查询对象属性
    DictNode* attr_node = dict_get_node(instance->dict, key);
    if (attr_node != NULL) {
        return attr_node->val;
    }

    if (instance->method_cache != NULL) {
        // 查询方法缓存
        DictNode* method = dict_get_node(instance->method_cache, key);
        if (method != NULL) {
            return method->val;
        }
    }

    // 查询类属性
    MpClass* klass = instance->klass;
    assert (klass != NULL);

    DictNode* class_attr_node = dict_get_node(klass->attr_dict, key);
    if (class_attr_node != NULL) {
        if (IS_FUNC(class_attr_node->val)) {
            if (instance->method_cache==NULL) {
                instance->method_cache = dict_new_ptr();
            }
            MpObj method = method_new(class_attr_node->val, self);
            dict_set0(instance->method_cache, key, method);
            return method;
        }
        return class_attr_node->val;
    }

    mp_raise("AttributeError: object has no attribute %o", key);
    return NONE_OBJECT;
}

MpObj class_new(MpObj name, MpObj module) {
    // TODO add class type
    assert(IS_STR(name));
    MpClass* klass = mp_malloc(sizeof(MpClass), "class.new");
    klass->name = name.value.str;
    klass->attr_dict = dict_new_ptr();
    klass->__init__ = NONE_OBJECT;
    klass->getattr_method = mp_new_native_func_obj(module, instance_getattr);
    klass->setattr_method = mp_new_native_func_obj(module, instance_setattr);
    klass->contains_method = NONE_OBJECT;
    klass->len_method = NONE_OBJECT;

    return gc_track(mp_to_obj(TYPE_CLASS, klass));
}

void class_set_attr(MpClass* klass, MpObj key, MpObj value) {
    if (obj_eq_cstr(key, "__init__")) {
        klass->__init__ = value;
    }

    dict_set0(klass->attr_dict, key, value);
}

MpObj class_new_by_cstr(char* name, MpObj module) {
    MpObj name_obj = string_new(name);
    return class_new(name_obj, module);
}

MpInstance* class_instance(MpClass* pclass) {
    MpDict* cl = pclass->attr_dict;
    MpInstance* instance = mp_malloc(sizeof(MpInstance), "class_instance");
    instance->klass = pclass;
    instance->dict = dict_new_ptr();
    instance->method_cache = NULL;

    MpObj obj_instance = mp_to_obj(TYPE_INSTANTCE, instance);
    gc_track(obj_instance);

    if (!IS_NONE(pclass->__init__)) {
        MpObj method = method_new(pclass->__init__, obj_instance);
        MP_CALL_EX(method);
    }

    return instance;
}

void class_free(MpClass* pclass) {
    mp_free(pclass, sizeof(MpClass));
}

void class_format(char* dest, MpObj class_obj) {
    mp_assert_type(class_obj, TYPE_CLASS, "class_format");
    MpClass* klass = GET_CLASS(class_obj);
    sprintf(dest, "<class '%s' at %p>", klass->name->value, klass);
}

MpObj mp_format_instance(MpInstance* instance) {
    assert(instance != NULL);
    char fmt[100];
    // todo module name
    char* klass_name = instance->klass->name->value;
    sprintf(fmt, "<%s object at %p>", klass_name, instance);
    return string_new(fmt);
}

int mp_is_in_instance(MpInstance* instance, MpObj key) {
    assert(instance != NULL);
    
    // 查询对象属性
    DictNode* attr_node = dict_get_node(instance->dict, key);
    if (attr_node != NULL) {
        return 1;
    }
    // 查询类属性
    MpClass* klass = instance->klass;
    assert (klass != NULL);

    DictNode* class_attr_node = dict_get_node(klass->attr_dict, key);
    if (class_attr_node != NULL) {
        return 1;
    }
    return 0;
}