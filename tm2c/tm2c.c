#include "../src/vm.c"

TmFrame* obj_getframe(int fidx) {
    if (fidx < 1 || fidx > FRAMES_COUNT) {
        tm_raise("obj_getframe:invalid fidx %d", fidx);
    }
    return tm->frames + fidx;
}

Object obj_getlocal(int fidx, int lidx) {
    TmFrame* f = obj_getframe(fidx);
    if (lidx < 0 || lidx >= f->maxlocals) {
        tm_raise("obj_getlocal:invalid lidx %d, maxlocals=%d", lidx, f->maxlocals);
    }
    return f->locals[lidx];
}

Object obj_getstack(int fidx, int sidx) {
    TmFrame* f = obj_getframe(fidx);
    int stacksize = f->top - f->stack;
    if (sidx < 0 || sidx >= stacksize) {
        tm_raise("obj_getstack:invalid sidx %d, stacksize=%d", sidx, stacksize);
    }
    return f->stack[sidx];
}

Object tm_getfname(Object fnc) {
    if (NOT_FUNC(fnc)) {
        tm_raise("tm_getfname expect function");
    }
    return GET_MODULE(GET_FUNCTION(fnc)->mod)->file;
}

void tm_setattr(Object dict, char* attr, Object value) {
    dict_set0(GET_DICT(dict), sz_to_string(attr), value);
}

Object tm_call(int lineno, Object func, int args, ...) {
    int i = 0;
    va_list ap;
    va_start(ap, args);
    arg_start();
    for (i = 0; i < args; i++) {
        arg_push(va_arg(ap, Object));
    }
    va_end(ap);
    // tm_printf("at line %d, try to call %o with %d args\n", lineno, get_func_name_obj(func), args);
    return call_function(func);
}

/*
Object tm_call_native(int lineno, Object (*fn)(int, va_list), int args, ...) {
    int i = 0;
    va_list ap;
    va_start(ap, args);
    arg_start();
    for (i = 0; i < args; i++) {
        arg_push(va_arg(ap, Object));
    }
    va_end(ap);
    va_start(ap, args);
    Object ret = fn(args, ap);
    va_end(ap);
    return ret;
}*/

Object tm_call_native(int lineno, Object (*fn)(), int args, ...) {
    int i = 0;
    va_list ap;
    va_start(ap, args);
    arg_start();
    for (i = 0; i < args; i++) {
        arg_push(va_arg(ap, Object));
    }
    va_end(ap);
    Object ret = fn();
    return ret;
}

void def_func(Object globals, Object name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = name;
    obj_set(globals,name, func);
}

void def_method(Object dict, Object name, Object (*native)()) {
    Object func = func_new(NONE_OBJECT, NONE_OBJECT, native);
    Object method = method_new(func, dict);
    obj_set(dict, name, method);
}

Object tm_take_arg() {
    return arg_take_obj("getarg");
}

void tm_def_mod(char* fname, Object mod) {
    Object o_name = sz_to_string(fname);
    obj_set(tm->modules, o_name, mod);
}

void tm_import_all(Object globals, Object mod_name) {
    int b_has = obj_in(mod_name, tm->modules);
    if (b_has) {
        Object mod_value = obj_get(tm->modules, mod_name);
        // do something here.
    }
}


Object bf_vmopt() {
    char* opt = arg_take_sz("vminfo");
    if (strcmp(opt, "gc") == 0) {
        gc_full();
    } else if (strcmp(opt, "help") == 0) {
        return sz_to_string("gc, help");
    } else if (strcmp(opt, "frame.local") == 0) {
        int fidx = arg_take_int("vminfo");
        int lidx = arg_take_int("vminfo");
        return obj_getlocal(fidx, lidx);
    } else if (strcmp(opt, "frame.stack") == 0) {
        int fidx = arg_take_int("vminfo");
        int sidx = arg_take_int("vminfo");
        return obj_getstack(fidx, sidx);
    } else if (strcmp(opt, "frame.index") == 0) {
        return tm_number(tm->frame-tm->frames);
    } else if (strcmp(opt, "frame.info") == 0) {
        int fidx = arg_take_int("vminfo");
        TmFrame *f = obj_getframe(fidx);
        Object info = dict_new();
        tm_setattr(info, "maxlocals", tm_number(f->maxlocals));
        tm_setattr(info, "stacksize", tm_number(f->top - f->stack));
        tm_setattr(info, "func", f->fnc);
        tm_setattr(info, "fname", tm_getfname(f->fnc));
        tm_setattr(info, "lineno", tm_number(f->lineno));
        return info;
    } else {
        tm_raise("invalid opt %s", opt);
    }
    return NONE_OBJECT;
}

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