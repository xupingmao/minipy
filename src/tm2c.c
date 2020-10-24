#ifndef TM2C_C
#define TM2C_C

Object mp_call(Object func, int args, ...) {
    int i = 0;
    va_list ap;
    va_start(ap, args);
    arg_start();
    for (i = 0; i < args; i++) {
        arg_push(va_arg(ap, Object));
    }
    va_end(ap);
    // mp_printf("at line %d, try to call %o with %d args\n", lineno, get_func_name_obj(func), args);
    // *self* will be resolved in call_function
    Object ret;
    if (IS_DICT(func)) {
        ret = class_new(func);
        Object *_fnc = dict_get_by_str(ret, "__init__");
        if (_fnc != NULL) {
            func = *_fnc;
        } else {
            goto mp_call_end;
        }
    }

    if (IS_FUNC(func)) {
        resolve_method_self(func);
        /* call native */
        if (GET_FUNCTION(func)->native != NULL) {
            ret = GET_FUNCTION(func)->native();
            goto mp_call_end;
        } else {
            mp_raise("can not call py function from tm2c");
        }
    } 

    mp_raise("File %o, line=%d: call_function:invalid object %o", GET_FUNCTION_FILE(tm->frame->fnc), 
        tm->frame->lineno, func);

    mp_call_end:
    gc_local_add(ret);
    return ret;
}

/**
 * call native function
 * @param fn, native function
 * @args, number of arguments
 * @...,  arguments
 * 
 * eg. add('test', wrapper(1))
 *  --> mp_call_native(add, 'test', mp_call_native(wrapper, 1), mp_call_native(wrapper, 2))
 *  <==> t1 = mp_call_native(wrapper, 1)  <local> = ['test', 1, 2]
 *       t1 = mp_call_native(wrapper, 2)  <local> = ['test', 1, 2]
 *       mp_call_native(add, t1, t2)      <local> = ['test', 1, 2]
 */
Object mp_call_native(Object (*fn)(), int args, ...) {
    int i = 0;
    va_list ap;
    Object obj_arg;

    va_start(ap, args);
    arg_start();
    for (i = 0; i < args; i++) {
        obj_arg = va_arg(ap, Object);
        arg_push(obj_arg);
    }
    va_end(ap);

    // record the length to restore
    int size = tm->local_obj_list->len; 

    Object ret = fn(); // execute native function

    gc_check_native_call(size, ret);

    return ret;
}

/**
 * optimization for mp_call_native
 * @since 2016-08-27
 */
Object mp_call_native_0(Object (*fn)()) {
    arg_start();
    int size = tm->local_obj_list->len;
    Object ret = fn();
    gc_check_native_call(size, ret);
}

Object mp_call_native_1(Object (*fn)(), Object arg1) {
    arg_start();
    arg_push(arg1);
    int size = tm->local_obj_list->len;
    Object ret = fn();
    gc_check_native_call(size, ret);
}

Object mp_call_native_2(Object (*fn)(), Object arg1, Object arg2) {
    arg_start();
    arg_push(arg1);
    arg_push(arg2);
    int size = tm->local_obj_list->len;
    Object ret = fn();
    gc_check_native_call(size, ret);
}

/**
 * call native function with debug info
 * @param lineno current lineno in python file
 * @param func_name function name
 */
Object mp_call_native_debug(int lineno, char* func_name, Object (*fn)(), int args, ...) {
    int i = 0;
    va_list ap;
    Object obj_arg;

    tm->frame->lineno = lineno;
    LOG(LEVEL_ERROR, "call,%d,%s,start", lineno, func_name, 0);
    va_start(ap, args);
    arg_start();
    for (i = 0; i < args; i++) {
        obj_arg = va_arg(ap, Object);
        arg_push(obj_arg);
    }
    va_end(ap);

    // mp_inspect_obj(get_mp_local_list());

    // record the length to restore
    int size = tm->local_obj_list->len; 
    Object ret = fn(); // execute native function
    LOG(LEVEL_ERROR, "call,%d,%s,end", lineno, func_name, 0);
    // if (size != tm->local_obj_list->len) {
        // if size changed, there must be new allocated objects
        gc_native_call_sweep(size, ret); // check whether need full gc.
    // }

    // mp_inspect_obj(get_mp_local_list());
    return ret;
}

void def_func(Object globals, Object name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = name;
    obj_set(globals,name, func);
}

void def_native_method(Object dict, Object name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    Object method = method_new(func, dict);
    obj_set(dict, name, method);
}

Object mp_take_arg() {
    return arg_take_obj("getarg");
}

void mp_def_mod(char* fname, Object mod) {
    Object o_name = sz_to_string(fname);
    obj_set(tm->modules, o_name, mod);
}

Object mp_import(Object globals, Object mod_name) {
    Object mod = obj_get(tm->modules, mod_name);
    obj_set(globals, mod_name, mod);
    return mod;
}

void mp_import_all(Object globals, Object mod_name) {
    int b_has = obj_in(mod_name, tm->modules);
    if (b_has) {
        Object mod_value = obj_get(tm->modules, mod_name);
        // do something here.
        mp_call_native_2(dict_builtin_update, globals, mod_value);
    }
}

/**
 * convert argv to list
 * @param n arguments count
 */
Object argv_to_list(int n, ...) {
    va_list ap;
    int i = 0;
    Object list = list_new(n);
    va_start(ap, n);
    for (i = 0; i < n; i++) {
        Object item = va_arg(ap, Object);
        obj_append(list, item);
    }
    va_end(ap);
    return list;
}

/**
 * convert argv to dict
 * @param n arguments count
 */
Object argv_to_dict(int n, ...) {
    va_list ap;
    int i = 0;
    Object dict = dict_new();
    va_start(ap, n);
    for (i = 0; i < n; i+=2) {
        Object key = va_arg(ap, Object);
        Object val = va_arg(ap, Object);
        obj_set(dict, key, val);
    }
    va_end(ap);
    return dict;
}


/** 
 * run c function
 * @param mod_name mocked module name
 */
int mp_run_func(int argc, char* argv[], char* mod_name, Object (*func)(void)) {
    int ret = vm_init(argc, argv);
    int i;
    if (ret != 0) { 
        return ret;
    }
    tm->local_obj_list = list_new_untracked(100);
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        Object *sys  = dict_get_by_str(tm->modules, "sys");
        Object *_argv = dict_get_by_str(*sys, "argv");
        list_insert(GET_LIST(*_argv), 0, string_new(mod_name));

        mp_call_native(func,0);
        gc_full();
        // printf("tm->max_allocated = %d\n", tm->max_allocated);
        // printf("tm->allocated = %d\n", tm->allocated);
        // fgetc(stdin);

    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    vm_destroy();
    return 0;
}

/**
 * get attribute by char array
 * @since 2016-09-02
 */
Object tm2c_get(Object obj, char* key) {
    Object obj_key = string_new(key);
    return obj_get(obj, obj_key);
}

/**
 * get attribute by char array
 * @since 2016-09-02
 */
void tm2c_set(Object obj, char* key, Object value) {
    Object obj_key = string_new(key);
    obj_set(obj, obj_key, value);
}

// float tm2c_numval(Object num) {
//     return GET_NUM(num);
// }


#endif /* TM2C_C */