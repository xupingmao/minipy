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
    return MpInstance_getattr(instance, key, NULL);
}

MpObj MpInstance_getattr(MpInstance* instance, MpObj key, MpObj* default_) {
    // 1. 检查 __dict__
    // 2. 检查 class的属性
    // 3. 调用 __getattr__
    assert(instance != NULL);
    MpClass* klass = instance->klass;
    assert (klass != NULL);

    // 1. 查询对象属性
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
    
    // 2. 查询类属性
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

    // 3. 处理 __getattr__
    if (NOT_NONE(klass->__getattr__)) {
        MpObj self = mp_to_obj(TYPE_INSTANTCE, instance);
        if (default_ == NULL) {
            MpObj args[2] = {self, key};
            return mp_call_obj_safe(klass->__getattr__, 2, args);
        } else {
            MpObj args[3] = {self, key, *default_};
            return mp_call_obj_safe(klass->__getattr__, 3, args);
        }
    }

    mp_raise("AttributeError: object has no attribute %o", key);
    return NONE_OBJECT;
}


void MpInstance_setattr(MpInstance* instance, MpObj key, MpObj value) {
    assert(instance != NULL);
    MpClass* klass = instance->klass;
    assert (klass != NULL);

    // 处理 __setattr__
    if (NOT_NONE(klass->__setattr__)) {
        MpObj self = mp_to_obj(TYPE_INSTANTCE, instance);
        MpObj args[3] = {self, key, value};
        mp_call_obj_safe(klass->__setattr__, 3, args);
        return;
    }

    // 更新对象属性
    dict_set0(instance->dict, key, value);
}

int MpInstance_len(MpInstance* instance) {
    assert(instance != NULL);
    MpClass* klass = instance->klass;
    assert (klass != NULL);

    if (NOT_NONE(klass->__len__)) {
        MpObj self = mp_to_obj(TYPE_INSTANTCE, instance);
        MpObj args[1] = {self};
        MpObj result = mp_call_obj_safe(klass->__len__, 1, args);
        return mp_toInt(result);
    }

    mp_raise("mp_len: %o has no attribute len", MpInstance_str(instance));
    return 0;
}

MpObj class_new(MpObj name, MpModule* module) {
    // TODO add class type
    assert(IS_STR(name));
    assert(module != NULL);

    MpClass* klass = mp_malloc(sizeof(MpClass), "class.new");
    klass->module = module;
    klass->name = name.value.str;
    klass->attr_dict = dict_new_ptr();
    // malloc会默认清零
    // klass->__init__ = NONE_OBJECT;
    // klass->__getattr__ = NONE_OBJECT;
    // klass->__setattr__ = NONE_OBJECT;
    // klass->__len__ = NONE_OBJECT;
    // klass->__str__ = NONE_OBJECT;
    // klass->__contains__ = NONE_OBJECT;

    return gc_track(mp_to_obj(TYPE_CLASS, klass));
}

void MpClass_setattr(MpClass* klass, MpObj key, MpObj value) {
    if (IS_STR(key)) {
        // TODO 优化遍历性能
        MpStr* attr = GET_STR_OBJ(key);
        if (string_eq_cstr(attr, "__init__")) {
            klass->__init__ = value;
        } else if (string_eq_cstr(attr, "__contains__")) {
            klass->__contains__ = value;
        } else if (string_eq_cstr(attr, "__setattr__")) {
            klass->__setattr__ = value;
        } else if (string_eq_cstr(attr, "__getattr__")) {
            klass->__getattr__ = value;
        } else if (string_eq_cstr(attr, "__len__")) {
            klass->__len__ = value;
        } else if (string_eq_cstr(attr, "__str__")) {
            klass->__str__ = value;
        }
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

MpObj MpInstance_str(MpInstance* instance) {
    assert(instance != NULL);
    assert(instance->klass != NULL);

    MpObj self = mp_to_obj(TYPE_INSTANTCE, instance);

    if (!IS_NONE(instance->klass->__str__)) {
        MpObj func = instance->klass->__str__;
        MpObj args[1] = {self};
        return mp_call_obj_safe(func, 1, args);
    }

    char fmt[100];
    // todo module name
    char* klass_name = instance->klass->name->value;
    sprintf(fmt, "<%s object at %p>", klass_name, instance);
    return string_new(fmt);
}

int MpInstance_contains(MpInstance* instance, MpObj key) {
    // 1. 检查 __contains__ 方法
    // 2. 检查 __iter__ (目前没实现, 先遍历 __dict__ 和父类) 
    // 3. 检查 __getitem__ 没有实现
    assert(instance != NULL);
    MpClass* klass = instance->klass;
    assert (klass != NULL);

    // 查询自定义的__contains__方法
    if (NOT_NONE(klass->__contains__)) {
        MpObj self = mp_to_obj(TYPE_INSTANTCE, instance);
        MpObj method = method_new(instance->klass->__contains__, self);
        MpObj result = MP_CALL_EX(method);
        return mp_obj_to_bool(result);
    }

    // 查询对象属性
    DictNode* attr_node = dict_get_node(instance->dict, key);
    if (attr_node != NULL) {
        return 1;
    }

    // 查询类属性
    DictNode* class_attr_node = dict_get_node(klass->attr_dict, key);
    if (class_attr_node != NULL) {
        return 1;
    }
    return 0;
}

void MpClass_RegNativeMethod(MpClass* clazz, char* name, MpNativeFunc native) {
    MpDict* attr_dict = clazz->attr_dict;
    MpObj func = func_new(tm->builtins_mod, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_new(name);
    MpClass_setattr(clazz, string_from_cstr(name), func);
}

MpObj MpClass_ToObj(MpClass* class_) {
    assert (class_ != NULL);
    return mp_to_obj(TYPE_CLASS, class_);
}
