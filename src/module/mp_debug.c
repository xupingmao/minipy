#include "../include/mp.h"
#include <math.h>

/***********************************
* built-in functions for developers
***********************************/

typedef struct TypeAndDouble_t {
    char type;
    double value;
} TypeAndDouble;


MpObj bf_inspect_ptr() {
    double _ptr = arg_take_double("inspect_ptr");
    int idx = arg_take_int("inspect_ptr");
    char* ptr = (char*)(long long)_ptr;
    return string_chr(ptr[idx]);
}

MpObj bf_get_current_frame() {
    MpObj frame_info = dict_new();
    obj_set_by_cstr(frame_info, "function", tm->frame->fnc);
    // obj_set_by_cstr(frame_info, "pc", number_obj((long long)tm->frame->pc));
    obj_set_by_cstr(frame_info, "index", number_obj((long long) (tm->frame - tm->frames)));
    return frame_info;
}

MpObj bf_get_vm_info() {
    MpObj mp_info = dict_new();
    obj_set_by_cstr(mp_info, "name", string_new("tm"));
    obj_set_by_cstr(mp_info, "vm_size", number_obj(sizeof(MpVm)));
    obj_set_by_cstr(mp_info, "obj_size", number_obj(sizeof(MpObj)));
    obj_set_by_cstr(mp_info, "obj_value_size", number_obj(sizeof(MpValue)));
    obj_set_by_cstr(mp_info, "type_and_double_size", number_obj(sizeof(TypeAndDouble)));
    obj_set_by_cstr(mp_info, "int_size", number_obj(sizeof(int)));
    obj_set_by_cstr(mp_info, "long_size", number_obj(sizeof(long)));
    obj_set_by_cstr(mp_info, "long_long_size", number_obj(sizeof(long long)));
    obj_set_by_cstr(mp_info, "float_size", number_obj(sizeof(float)));
    obj_set_by_cstr(mp_info, "double_size", number_obj(sizeof(double)));
    obj_set_by_cstr(mp_info, "jmp_buf_size", number_obj(sizeof(jmp_buf)));
    obj_set_by_cstr(mp_info, "code_cache_size", number_obj(sizeof(MpCodeCache)));
    obj_set_by_cstr(mp_info, "total_obj_len", number_obj(tm->all->len));
    obj_set_by_cstr(mp_info, "alloc_mem", number_obj(tm->allocated));
    obj_set_by_cstr(mp_info, "gc_threshold", number_obj(tm->gc_threshold));
    obj_set_by_cstr(mp_info, "frame_index", number_obj(tm->frame - tm->frames));
    obj_set_by_cstr(mp_info, "consts_len", number_obj(tm->constants->len));
    return mp_info;
}


MpFrame* obj_getframe(int fidx) {
    if (fidx < 1 || fidx > FRAMES_COUNT) {
        mp_raise("obj_getframe:invalid fidx %d", fidx);
    }
    return tm->frames + fidx;
}

MpObj obj_getlocal(int fidx, int lidx) {
    MpFrame* f = obj_getframe(fidx);
    if (lidx < 0 || lidx >= f->maxlocals) {
        mp_raise("obj_getlocal:invalid lidx %d, maxlocals=%d", lidx, f->maxlocals);
    }
    return f->locals[lidx];
}

MpObj obj_getstack(int fidx, int sidx) {
    MpFrame* f = obj_getframe(fidx);
    int stacksize = f->top - f->stack;
    if (sidx < 0 || sidx >= stacksize) {
        mp_raise("obj_getstack:invalid sidx %d, stacksize=%d", sidx, stacksize);
    }
    return f->stack[sidx];
}

MpObj mp_getfname(MpObj fnc) {
    if (NOT_FUNC(fnc)) {
        mp_raise("mp_getfname expect function");
    }
    return GET_MODULE(GET_FUNCTION(fnc)->mod)->file;
}

MpObj bf_get_mp_local_list() {
    MpObj obj;
    MP_TYPE(obj) = TYPE_LIST;
    GET_LIST(obj) = tm->local_obj_list;
    return obj;
}


MpObj bf_set_vm_state() {
    int state = arg_take_int("set_vm_state");
    switch(state) {
        case 0:tm->debug = 0;break;
        case 1:tm->debug = 1;break;
    }
    return NONE_OBJECT;
}

MpObj bf_vmopt() {
    char* opt = arg_take_cstr("vminfo");
    if (strcmp(opt, "gc") == 0) {
        gc_full();
    } else if (strcmp(opt, "help") == 0) {
        return string_from_cstr("gc, help");
    } else if (strcmp(opt, "frame.local") == 0) {
        int fidx = arg_take_int("vminfo");
        int lidx = arg_take_int("vminfo");
        return obj_getlocal(fidx, lidx);
    } else if (strcmp(opt, "frame.stack") == 0) {
        int fidx = arg_take_int("vminfo");
        int sidx = arg_take_int("vminfo");
        return obj_getstack(fidx, sidx);
    } else if (strcmp(opt, "frame.index") == 0) {
        return number_obj(tm->frame-tm->frames);
    } else if (strcmp(opt, "frame.info") == 0) {
        int fidx = arg_take_int("vminfo");
        MpFrame *f = obj_getframe(fidx);
        MpObj info = dict_new();
        mp_setattr(info, "maxlocals", number_obj(f->maxlocals));
        mp_setattr(info, "stacksize", number_obj(f->top - f->stack));
        mp_setattr(info, "func", f->fnc);
        mp_setattr(info, "fname", mp_getfname(f->fnc));
        mp_setattr(info, "lineno", number_obj(f->lineno));
        return info;
    } else {
        mp_raise("invalid opt %s", opt);
    }
    return NONE_OBJECT;
}

MpObj bf_print_dict_info() {
    MpDict* dict = arg_take_dict_ptr("debug.print_debug_info");
    dict_print_debug_info(dict);
    return NONE_OBJECT;
}


/**
 * init debug module
 * @since 2016-11-16
 */
void init_mod_for_debug() {
    MpObj debug = dict_new();
    
    reg_mod("debug", debug);
    reg_mod_func(debug, "get_ex_list", bf_get_ex_list);
    reg_mod_func(debug, "set_vm_state", bf_set_vm_state);
    reg_mod_func(debug, "inspect_ptr", bf_inspect_ptr);
    reg_mod_func(debug, "get_current_frame", bf_get_current_frame);
    reg_mod_func(debug, "get_vm_info", bf_get_vm_info);
    reg_mod_func(debug, "get_mp_local_list", bf_get_mp_local_list);
    reg_mod_func(debug, "vmopt", bf_vmopt);
    reg_mod_func(debug, "print_dict_info", bf_print_dict_info);
}

