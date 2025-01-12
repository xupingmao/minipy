#include "include/class_.h"
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
    return mp_get_instance_attr(instance, key);
}

MpObj mp_get_instance_attr(MpInstance* instance, MpObj key) {
    assert(instance != NULL);
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
            MpObj method = method_new(class_attr_node->val, mp_to_obj(TYPE_INSTANTCE, instance));
            dict_set0(instance->method_cache, key, method);
            return method;
        }
        return class_attr_node->val;
    }

    if (obj_eq_cstr(key, "__class__")) {
        return mp_to_obj(TYPE_CLASS, instance->klass);
    }
    if (obj_eq_cstr(key, "__dict__")) {
        return mp_to_obj(TYPE_DICT, instance->dict);
    }

    mp_raise("AttributeError: object has no attribute %o", key);
    return NONE_OBJECT;
}

MpObj class_new(MpObj name, MpModule* module) {
    // TODO add class type
    assert(IS_STR(name));
    assert(module != NULL);

    MpClass* klass = mp_malloc(sizeof(MpClass), "class.new");
    klass->module = module;
    klass->name = name.value.str;
    klass->attr_dict = dict_new_ptr();
    klass->__init__ = NONE_OBJECT;
    klass->getattr_method = mp_new_native_func_obj(module, instance_getattr);
    klass->setattr_method = mp_new_native_func_obj(module, instance_setattr);
    klass->contains_method = NONE_OBJECT;
    klass->len_method = NONE_OBJECT;
    klass->__str__ = NONE_OBJECT;

    return gc_track(mp_to_obj(TYPE_CLASS, klass));
}

void class_set_attr(MpClass* klass, MpObj key, MpObj value) {
    if (obj_eq_cstr(key, "__init__")) {
        klass->__init__ = value;
    }

    dict_set0(klass->attr_dict, key, value);
}

MpObj class_new_by_cstr(char* name, MpModule* module) {
    MpObj name_obj = string_new(name);
    return class_new(name_obj, module);
}

MpClass* MpClass_New(char* name, MpModule* module) {
    assert (name != NULL);
    assert (module != NULL);

    MpObj klass = class_new_by_cstr(name, module);
    return GET_CLASS(klass);
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

MpObj mp_format_class(MpClass* klass) {
    char dest[100];
    assert(klass != NULL);
    MpModule* module = klass->module;
    sprintf(dest, "<class '%s.%s' at %p>", module->file->value, klass->name->value, klass);
    return string_new(dest);
}

MpObj mp_format_instance(MpInstance* instance) {
    assert(instance != NULL);
    assert(instance->klass != NULL);

    MpObj self = mp_to_obj(TYPE_INSTANTCE, instance);

    if (!IS_NONE(instance->klass->__str__)) {
        MpObj method = method_new(instance->klass->__str__, self);
        return MP_CALL_EX(method);
    }

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

void MpClass_RegNativeMethod(MpClass* clazz, char* name, MpObj (*native)()) {
    MpDict* attr_dict = clazz->attr_dict;
    MpObj func = func_new(tm->builtins_mod, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_new(name);
    dict_set_by_cstr(attr_dict, name, func);
}

MpObj MpClass_ToObj(MpClass* class_) {
    assert (class_ != NULL);
    return mp_to_obj(TYPE_CLASS, class_);
}
