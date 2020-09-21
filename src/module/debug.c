#include "../include/mp.h"
#include <math.h>

/***********************************
* built-in functions for developers
***********************************/

Object bf_inspect_ptr() {
    double _ptr = arg_take_double("inspect_ptr");
    int idx = arg_take_int("inspect_ptr");
    char* ptr = (char*)(long long)_ptr;
    return string_chr(ptr[idx]);
}

Object bf_get_current_frame() {
    Object frame_info = dict_new();
    dict_set_by_str(frame_info, "function", tm->frame->fnc);
    // dict_set_by_str(frame_info, "pc", tm_number((long long)tm->frame->pc));
    dict_set_by_str(frame_info, "index", tm_number((long long) (tm->frame - tm->frames)));
    return frame_info;
}

Object bf_get_vm_info() {
    Object tm_info = dict_new();
    dict_set_by_str(tm_info, "name", string_new("tm"));
    dict_set_by_str(tm_info, "vm_size", tm_number(sizeof(TmVm)));
    dict_set_by_str(tm_info, "obj_size", tm_number(sizeof(Object)));
    dict_set_by_str(tm_info, "int_size", tm_number(sizeof(int)));
    dict_set_by_str(tm_info, "long_size", tm_number(sizeof(long)));
    dict_set_by_str(tm_info, "long_long_size", tm_number(sizeof(long long)));
    dict_set_by_str(tm_info, "float_size", tm_number(sizeof(float)));
    dict_set_by_str(tm_info, "double_size", tm_number(sizeof(double)));
    dict_set_by_str(tm_info, "jmp_buf_size", tm_number(sizeof(jmp_buf)));
    dict_set_by_str(tm_info, "total_obj_len", tm_number(tm->all->len));
    dict_set_by_str(tm_info, "alloc_mem", tm_number(tm->allocated));
    dict_set_by_str(tm_info, "gc_threshold", tm_number(tm->gc_threshold));
    dict_set_by_str(tm_info, "frame_index", tm_number(tm->frame - tm->frames));
    dict_set_by_str(tm_info, "consts_len", tm_number(DICT_LEN(tm->constants)));
    return tm_info;
}


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

Object bf_get_tm_local_list() {
    Object obj;
    TM_TYPE(obj) = TYPE_LIST;
    GET_LIST(obj) = tm->local_obj_list;
    return obj;
}


Object bf_set_vm_state() {
    int state = arg_take_int("set_vm_state");
    switch(state) {
        case 0:tm->debug = 0;break;
        case 1:tm->debug = 1;break;
    }
    return NONE_OBJECT;
}

Object bf_vmopt() {
    char* opt = arg_take_sz("vminfo");
    if (strcmp(opt, "gc") == 0) {
        gc_full();
    } else if (strcmp(opt, "help") == 0) {
        return string_from_sz("gc, help");
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

/**
 * init debug module
 * @since 2016-11-16
 */
void debug_mod_init() {
    Object debug = dict_new();
    
    reg_mod_func(debug, "get_ex_list", bf_get_ex_list);
    reg_mod_func(debug, "set_vm_state", bf_set_vm_state);
    reg_mod_func(debug, "inspect_ptr", bf_inspect_ptr);
    reg_mod_func(debug, "get_current_frame", bf_get_current_frame);
    reg_mod_func(debug, "get_vm_info", bf_get_vm_info);
    reg_mod_func(debug, "get_tm_local_list", bf_get_tm_local_list);
    reg_mod_func(debug, "vmopt", bf_vmopt);

    dict_set_by_str(tm->modules, "debug", debug);
}