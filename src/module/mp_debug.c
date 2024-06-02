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
    double _ptr = mp_take_double_arg("inspect_ptr");
    int idx = mp_take_int_arg("inspect_ptr");
    char* ptr = (char*)(long long)_ptr;
    return string_chr(ptr[idx]);
}

static MpObj bf_get_current_frame() {
    MpObj frame_info = dict_new();
    obj_set_by_cstr(frame_info, "function", tm->frame->fnc);
    // obj_set_by_cstr(frame_info, "pc", mp_number((long long)tm->frame->pc));
    obj_set_by_cstr(frame_info, "index", mp_number((long long) (tm->frame - tm->frames)));
    return frame_info;
}

static MpObj bf_get_vm_info() {
    MpObj mp_info = dict_new();
    obj_set_by_cstr(mp_info, "name", string_new("tm"));
    obj_set_by_cstr(mp_info, "vm_size", mp_number(sizeof(MpVm)));
    obj_set_by_cstr(mp_info, "obj_size", mp_number(sizeof(MpObj)));
    obj_set_by_cstr(mp_info, "obj_value_size", mp_number(sizeof(MpValue)));
    obj_set_by_cstr(mp_info, "type_and_double_size", mp_number(sizeof(TypeAndDouble)));
    obj_set_by_cstr(mp_info, "int_size", mp_number(sizeof(int)));
    obj_set_by_cstr(mp_info, "long_size", mp_number(sizeof(long)));
    obj_set_by_cstr(mp_info, "long_long_size", mp_number(sizeof(long long)));
    obj_set_by_cstr(mp_info, "float_size", mp_number(sizeof(float)));
    obj_set_by_cstr(mp_info, "double_size", mp_number(sizeof(double)));
    obj_set_by_cstr(mp_info, "jmp_buf_size", mp_number(sizeof(jmp_buf)));
    obj_set_by_cstr(mp_info, "code_cache_size", mp_number(sizeof(MpCodeCache)));
    obj_set_by_cstr(mp_info, "mp_str_size", mp_number(sizeof(MpStr)));
    obj_set_by_cstr(mp_info, "mp_func_size", mp_number(sizeof(MpFunction)));
    obj_set_by_cstr(mp_info, "mp_dict_size", mp_number(sizeof(MpDict)));
    obj_set_by_cstr(mp_info, "total_obj_len", mp_number(tm->all->len));
    obj_set_by_cstr(mp_info, "alloc_mem", mp_number(tm->allocated));
    obj_set_by_cstr(mp_info, "gc_threshold", mp_number(tm->gc_threshold));
    obj_set_by_cstr(mp_info, "frame_index", mp_number(tm->frame - tm->frames));
    obj_set_by_cstr(mp_info, "consts_len", mp_number(tm->constants->len));
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

    obj_set_by_cstr(mp_info, "cache_size", mp_number(cache_size));
    obj_set_by_cstr(mp_info, "gc_all_size", mp_number(list_sizeof(tm->all)));
    obj_set_by_cstr(mp_info, "constants_size", mp_number(dict_sizeof(tm->constants)));
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
    MpModule*module = GET_MODULE(GET_FUNCTION(fnc)->mod);
    return mp_to_obj(TYPE_STR, module->file);
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
    int state = mp_take_int_arg("set_vm_state");
    switch(state) {
        case 0:tm->debug = 0;break;
        case 1:tm->debug = 1;break;
    }
    return NONE_OBJECT;
}

static MpObj bf_vmopt() {
    char* opt = mp_take_cstr_arg("vminfo");
    if (strcmp(opt, "gc") == 0) {
        gc_full();
    } else if (strcmp(opt, "help") == 0) {
        return string_from_cstr("gc, help");
    } else if (strcmp(opt, "frame.local") == 0) {
        int fidx = mp_take_int_arg("vminfo");
        int lidx = mp_take_int_arg("vminfo");
        return obj_getlocal(fidx, lidx);
    } else if (strcmp(opt, "frame.stack") == 0) {
        int fidx = mp_take_int_arg("vminfo");
        int sidx = mp_take_int_arg("vminfo");
        return obj_getstack(fidx, sidx);
    } else if (strcmp(opt, "frame.index") == 0) {
        return mp_number(tm->frame-tm->frames);
    } else if (strcmp(opt, "frame.info") == 0) {
        int fidx = mp_take_int_arg("vminfo");
        MpFrame *f = obj_getframe(fidx);
        MpObj info = dict_new();
        mp_setattr(info, "maxlocals", mp_number(f->maxlocals));
        mp_setattr(info, "stacksize", mp_number(f->top - f->stack));
        mp_setattr(info, "func", f->fnc);
        mp_setattr(info, "fname", mp_getfname(f->fnc));
        mp_setattr(info, "lineno", mp_number(f->lineno));
        return info;
    } else {
        mp_raise("invalid opt %s", opt);
    }
    return NONE_OBJECT;
}

static MpObj bf_print_dict_info() {
    MpDict* dict = mp_take_dict_ptr_arg("debug.print_debug_info");
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
    MpObj debug = mp_new_native_module("mp_debug");
    MpModule_RegFunc(debug, "get_ex_list", bf_get_ex_list);
    MpModule_RegFunc(debug, "set_vm_state", bf_set_vm_state);
    MpModule_RegFunc(debug, "inspect_ptr", bf_inspect_ptr);
    MpModule_RegFunc(debug, "get_current_frame", bf_get_current_frame);
    MpModule_RegFunc(debug, "get_vm_info", bf_get_vm_info);
    MpModule_RegFunc(debug, "get_memory_info", bf_get_memory_info);
    MpModule_RegFunc(debug, "get_mp_local_list", bf_get_mp_local_list);
    MpModule_RegFunc(debug, "vmopt", bf_vmopt);
    MpModule_RegFunc(debug, "print_dict_info", bf_print_dict_info);
}

