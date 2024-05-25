#include "../include/mp.h"
#include <math.h>

/***********************************
* built-in functions for developers
***********************************/

typedef struct TypeAndDouble_t {
    char type;
    double value;
} TypeAndDouble;


static MpObj bf_inspect_ptr() {
    double _ptr = arg_take_double("inspect_ptr");
    int idx = arg_take_int("inspect_ptr");
    char* ptr = (char*)(long long)_ptr;
    return string_chr(ptr[idx]);
}

static MpObj bf_get_current_frame() {
    MpObj frame_info = dict_new();
    obj_set_by_cstr(frame_info, "function", tm->frame->fnc);
    // obj_set_by_cstr(frame_info, "pc", number_obj((long long)tm->frame->pc));
    obj_set_by_cstr(frame_info, "index", number_obj((long long) (tm->frame - tm->frames)));
    return frame_info;
}

static MpObj bf_get_vm_info() {
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
    obj_set_by_cstr(mp_info, "mp_str_size", number_obj(sizeof(MpStr)));
    obj_set_by_cstr(mp_info, "mp_func_size", number_obj(sizeof(MpFunction)));
    obj_set_by_cstr(mp_info, "mp_dict_size", number_obj(sizeof(MpDict)));
    obj_set_by_cstr(mp_info, "total_obj_len", number_obj(tm->all->len));
    obj_set_by_cstr(mp_info, "alloc_mem", number_obj(tm->allocated));
    obj_set_by_cstr(mp_info, "gc_threshold", number_obj(tm->gc_threshold));
    obj_set_by_cstr(mp_info, "frame_index", number_obj(tm->frame - tm->frames));
    obj_set_by_cstr(mp_info, "consts_len", number_obj(tm->constants->len));
    return mp_info;
}

static MpObj bf_get_memory_info() {
    MpObj mp_info = dict_new();
    size_t cache_size = 0;
    MpList* all = tm->all;
    int i = 0;

    for (i = 0; i < all->len; i++) {
        MpObj node = all->nodes[i];
        if (node.type == TYPE_MODULE) {
            if (GET_MODULE(node)->cache != NULL) {
                cache_size += sizeof(MpCodeCache) * GET_MODULE(node)->cache_cap;
            }
        }
    }

    obj_set_by_cstr(mp_info, "cache_size", number_obj(cache_size));
    obj_set_by_cstr(mp_info, "gc_all_size", number_obj(list_sizeof(tm->all)));
    obj_set_by_cstr(mp_info, "constants_size", number_obj(dict_sizeof(tm->constants)));
    return mp_info;
}


static MpFrame* obj_getframe(int fidx) {
    if (fidx < 1 || fidx > FRAMES_COUNT) {
        mp_raise("obj_getframe:invalid fidx %d", fidx);
    }
    return tm->frames + fidx;
}

static MpObj obj_getlocal(int fidx, int lidx) {
    MpFrame* f = obj_getframe(fidx);
    if (lidx < 0 || lidx >= f->maxlocals) {
        mp_raise("obj_getlocal:invalid lidx %d, maxlocals=%d", lidx, f->maxlocals);
    }
    return f->locals[lidx];
}

static MpObj obj_getstack(int fidx, int sidx) {
    MpFrame* f = obj_getframe(fidx);
    int stacksize = f->top - f->stack;
    if (sidx < 0 || sidx >= stacksize) {
        mp_raise("obj_getstack:invalid sidx %d, stacksize=%d", sidx, stacksize);
    }
    return f->stack[sidx];
}

static MpObj mp_getfname(MpObj fnc) {
    if (NOT_FUNC(fnc)) {
        mp_raise("mp_getfname expect function");
    }
    return GET_MODULE(GET_FUNCTION(fnc)->mod)->file;
}

static MpObj bf_get_mp_local_list() {
    MpObj obj;

    if (tm->local_obj_list == NULL) {
        return list_new(0);
    }

    MP_TYPE(obj) = TYPE_LIST;
    GET_LIST(obj) = tm->local_obj_list;
    return obj;
}


static MpObj bf_set_vm_state() {
    int state = arg_take_int("set_vm_state");
    switch(state) {
        case 0:tm->debug = 0;break;
        case 1:tm->debug = 1;break;
    }
    return NONE_OBJECT;
}

static MpObj bf_vmopt() {
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

static MpObj bf_print_dict_info() {
    MpDict* dict = arg_take_dict_ptr("debug.print_debug_info");
    dict_print_debug_info(dict);
    return NONE_OBJECT;
}

static MpObj bf_get_ex_list() {
    return tm->ex_list;
}


/**
 * init debug module
 * @since 2016-11-16
 */
void mp_debug_init() {    
    MpObj debug = mp_new_native_module("debug");
    mod_reg_func(debug, "get_ex_list", bf_get_ex_list);
    mod_reg_func(debug, "set_vm_state", bf_set_vm_state);
    mod_reg_func(debug, "inspect_ptr", bf_inspect_ptr);
    mod_reg_func(debug, "get_current_frame", bf_get_current_frame);
    mod_reg_func(debug, "get_vm_info", bf_get_vm_info);
    mod_reg_func(debug, "get_memory_info", bf_get_memory_info);
    mod_reg_func(debug, "get_mp_local_list", bf_get_mp_local_list);
    mod_reg_func(debug, "vmopt", bf_vmopt);
    mod_reg_func(debug, "print_dict_info", bf_print_dict_info);
}

