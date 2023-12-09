/**
 * 1. mark all allocated object to be unused (0);
 * 2. mark objects can be reached from `root` to be used (1);
 * 3. [at runtime] mark objects can be reached from `frames` to be used (1);
 * 4. [at native call] mark objects in tm->local_obj_list to be used(1);
 * 5. release objects which are marked unused (0).
 * 
 * @since 2015
 * @modified 2022/06/11 21:11:51
 */

#include "include/mp.h"
#include "log.c"

void gc_init_chars();
void gc_init_frames();
static const char* gc_mark_ex(MpObj, const char*);

#define GC_CONSTANS_LEN 10
#define GC_REACHED_SIGN 1
#define GC_MARKED(o)    (o).value.gc->marked
#define GET_CHAR(n)     ARRAY_CHARS[n]


/**
 * init garbage collector
 * @since 2016
 */
void gc_init() {    
    /* initialize constants */
    tm->_TRUE  = number_obj(1);
    tm->_FALSE = number_obj(0);
    NONE_OBJECT.type = TYPE_NONE;
    
    tm->init = 0;
    tm->debug = 0;
    tm->allocated = 0;
    tm->max_allocated = 0;
    tm->gc_threshold  = 1024 * 8; // set 8k to see gc process
    tm->gc_state = 1; // enable gc.

    tm->all = list_new_untracked(100);
    // object allocated in local scope. which can be sweeped simply.
    tm->local_obj_list = NULL;
    // tm->gc_deleted = NULL;
    tm->list_proto = NONE_OBJECT;
    tm->dict_proto = NONE_OBJECT;
    tm->str_proto  = NONE_OBJECT;
    
    tm->root = list_new(100);
    tm->builtins = dict_new();
    tm->modules = dict_new();
    tm->constants = dict_new_ptr();
    tm->builtins_mod = module_new(string_static("_builtins.py"), 
        string_static("_builtins"), 
        string_static(""));

    /* initialize exception */
    tm->ex_list = list_new(10);
    obj_append(tm->root, tm->ex_list);
    tm->ex = NONE_OBJECT;
    tm->ex_line = NONE_OBJECT;

    #ifdef RECORD_LAST_OP
        CodeQueue_Init(&tm->last_op_queue, 20);
    #endif
    
    /* initialize chars */
    gc_init_chars();
    
    /* initialize frames */
    gc_init_frames();
}

void gc_init_chars() {
    int i = 0;
    ARRAY_CHARS = list_new(256); // init global ARRAY_CHARS
    for (i = 0; i < 256; i++) {
        obj_append(ARRAY_CHARS, string_char_new(i));
    }
    obj_append(tm->root, ARRAY_CHARS);
}

void gc_init_frames() {
    int i = 0;
    tm->frames_init_done = 0;
    for (i = 0; i < FRAMES_COUNT; i++) {
        MpFrame* f = tm->frames + i;
        f->stack = tm->stack;
        f->top   = tm->stack;
        f->lineno = -1;
        f->fnc = NONE_OBJECT;
        f->pc = NULL;
        f->maxlocals = 0;
        f->maxstack  = 0;
        f->jmp = NULL;
        f->idx = i;
    }
    tm->frames_init_done = 1;
    tm->frame = tm->frames;
    
    // reset stack values
    memset(tm->stack, 0, sizeof(tm->stack));

    // set global mp_stack_end
    tm->stack_end = tm->stack + STACK_SIZE;
}


void* mp_malloc(size_t size) {
    void* block = NULL;
    MpObj* func = NULL;

    if (size <= 0) {
        mp_raise(
            "mp_malloc: attempts to allocate a memory block of size %d!", size);
        return NULL;
    }
    block = malloc(size);
    memset(block, 0, size);
    
    log_debug("mp_malloc: %p %d->%d +%d", 
        block, tm->allocated, tm->allocated + size, size);

    if (block == NULL) {
        mp_raise("mp_malloc: fail to malloc memory block of size %d", size);
    }
    tm->allocated += size;
    tm->max_allocated = max(tm->max_allocated, tm->allocated);

    return block;
}

void* mp_realloc(void* o, size_t osize, size_t nsize) {
    void* block = mp_malloc(nsize);
    memcpy(block, o, osize);
    mp_free(o, osize);
    return block;
}

void mp_free(void* ptr, size_t size) {
    if (ptr == NULL)
        return;

    log_debug("mp_free: %p %d->%d -%d", ptr, 
        tm->allocated, tm->allocated - size, size);
    
    memset(ptr, 0, size);

    free(ptr);
    tm->allocated -= size;
}

/**
 * tracking allocated object
 * @since 2014-??
 * @modified 2016-08-20
 * add to tm->local_obj_list, which will be assumed like frame-locals
 */
MpObj gc_track(MpObj v) {
    switch (v.type) {
        case TYPE_NUM:
        case TYPE_NONE:
            return v;
        case TYPE_STR:
            v.value.str->marked = 0;
            break;
        case TYPE_LIST:
            GET_LIST(v)->marked = 0;
            break;
        case TYPE_DICT:
            GET_DICT(v)->marked = 0;
            break;
        case TYPE_MODULE:
            GET_MODULE(v)->marked = 0;
            break;
        case TYPE_FUNCTION:
            GET_FUNCTION(v)->marked = 0;
            break;
        case TYPE_DATA:
            GET_DATA(v)->marked = 0;
            break;
        case TYPE_CLASS:
            GET_CLASS(v)->marked = 0;
            break;
        default:
            mp_raise("gc_track: unsupported type %d", v.type);
            return v;
    }
    if (tm->local_obj_list != NULL) {
        // if local-obj-sweep is enabled, add this to local list
        // if the object cant be recycled , move to all.
        list_append(tm->local_obj_list, v);
    }
    
    list_append(tm->all, v);
    return v;
}

const char* gc_mark_list(MpList* list) {
    if (list->marked)
        return NULL;
    list->marked = GC_REACHED_SIGN;
    int i;
    for (i = 0; i < list->len; i++) {
        const char* err = gc_mark_ex(list->nodes[i], "list");
        if (err != NULL) {
            printf("%s:%d/%s failed: length(%d)\n", __FILE__, __LINE__, __FUNCTION__, list->len);
            return err;
        }
    }
    return NULL;
}
 
void gc_mark_dict(MpDict* dict) {
    if (dict->marked)
        return;
    dict->marked = GC_REACHED_SIGN;
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used > 0) {
            gc_mark_ex(dict->nodes[i].key, "dict.key");
            gc_mark_ex(dict->nodes[i].val, "dict.val");
        }
    }
}

void gc_mark_func(MpFunction* func) {
    if (func->marked)
        return;
    func->marked = GC_REACHED_SIGN;
    gc_mark_ex(func->mod, "func.mod");
    gc_mark_ex(func->self, "func.self");
    gc_mark_ex(func->name, "func.name");
}

void gc_mark_class(MpClass* pclass) {
    if (pclass->marked) {
        return;
    }

    pclass->marked = GC_REACHED_SIGN;
    gc_mark_ex(pclass->name, "class.name");
    gc_mark_ex(pclass->attr_dict, "class.attr_dict");
}

/**
 * mark object only once, not cursively
 * @since 2016-08-21
 */
void gc_mark_single(MpObj o) {
    if (o.type == TYPE_NUM || o.type == TYPE_NONE) {
        return;
    }
    GC_MARKED(o) = 1;
}

void gc_mark_module(MpModule* pmodule) {
    if (pmodule->marked) {
        return;
    }

    pmodule->marked = GC_REACHED_SIGN;
    gc_mark_ex(pmodule->code, "module.code");
    gc_mark_ex(pmodule->file, "module.file");
    gc_mark_ex(pmodule->globals, "module.globals");
}

void gc_mark(MpObj o) {
    gc_mark_ex(o, "default");
}

/**
 * mark object as used
 * @since 2014-??
 */
static const char* gc_mark_ex(MpObj o, const char* source) {
    if (o.type == TYPE_NUM || o.type == TYPE_NONE)
        return NULL;
    switch (o.type) {
    case TYPE_STR: {
        if (o.value.str->marked)
            return NULL;
        o.value.str->marked = GC_REACHED_SIGN;
        break;
    }
    case TYPE_LIST:
        return gc_mark_list(GET_LIST(o));

    case TYPE_DICT:
        gc_mark_dict(GET_DICT(o));
        break;
    case TYPE_FUNCTION:
        gc_mark_func(GET_FUNCTION(o));
        break;
    case TYPE_CLASS:
        gc_mark_class(GET_CLASS(o));
        break;
    case TYPE_MODULE:
        gc_mark_module(GET_MODULE(o));
        break;
    case TYPE_DATA:
        if (GET_DATA(o)->marked)
            return NULL;
        GET_DATA(o)->marked = GC_REACHED_SIGN;
        GET_DATA(o)->mark(GET_DATA(o));
        // GET_DATA(o)->proto->mark(GET_DATA(o));
        break;
    default: {
            printf("%s:%d/%s unknown object type(%d), source(%s)\n", __FILE__, __LINE__, __FUNCTION__, o.type, source);
            MpObj error = mp_format("gc_mark_ex: unknown object type %d, source:%s", o.type, source);
            return STR_TO_CSTR(error);
        }
    }

    return NULL;
}

void gc_unmark(MpObj o) {
    if (o.type == TYPE_NUM || o.type == TYPE_NONE)
        return;
    GC_MARKED(o) = 0;
}


static void gc_print_frame_stack_info(MpFrame* f) {
    mp_printf("frame.func:%o\n", f->fnc);
    int index = 0;
    for(MpObj *temp = f->stack; temp <= f->top; temp++) {
        mp_printf("stack[%d]=%o\n", index, *temp);
        index++;
    }
}

void gc_mark_frames() {
    int i = 0;
    for(i = 0; i < STACK_SIZE; i++) {
        /* 局部变量和操作栈都在stack里面 */
        gc_mark_and_check(tm->stack[i], "vm.stack");
    }

    for (i = 0; i < FRAMES_COUNT; i++) {
        gc_mark_and_check(tm->frames[i].fnc, "frame.func");
    }
}

/**
 * mark objects in frame-local and frame-stack
 * @since 2015
 */
void gc_mark_frames_old() {
    MpFrame* f = NULL;
    int j = 0;

    // why frames + 1 ?
    for(f = tm->frames + 1; f <= tm->frame; f++) {
        gc_mark_and_check(f->fnc, "frame.func");
        /* mark locals */
        for(j = 0; j < f->maxlocals; j++) {
            gc_mark_and_check(f->locals[j], "frame.local");
        }

        /* mark operand stack */
        MpObj* temp;
        for(temp = f->stack; temp <= f->top; temp++) {
            // gc_mark_and_check(*temp, "frame.stack");
            const char* err = gc_mark_ex(*temp, "frame.stack");
            if (err != NULL) {
                gc_print_frame_stack_info(f);
                mp_raise("gc failed:%s", err);
            }
        }
    }
}

/**
 * sweep unmarked objects in tm->all
 * @since ? before 2016
 * @modified 2016-08-20
 */
void gc_sweep() {
    int n, i;
    
    int deleted_cnt = 0;
    int64_t start_time  = time_get_milli_seconds();

    log_info("gc_sweep: %d start", tm->all->len);
    MpList* temp = list_new_untracked(200);
    MpList* all = tm->all;

    for (i = 0; i < all->len; i++) {
        if (GC_MARKED(tm->all->nodes[i])) {
            list_append(temp, all->nodes[i]);
        } else {
            obj_free(all->nodes[i]);
            deleted_cnt+=1;
            // list_append(temp, all->nodes[i]);
        }
    }
    list_free(tm->all);
    tm->all = temp;

    int64_t cost_time = time_get_milli_seconds() - start_time;
    
    log_info("gc_sweep: delete objects: %d", deleted_cnt);
    log_info("gc_sweep: active objects: %d end, cost:%lldms", 
        tm->all->len, cost_time);
}

void gc_reset_all() {
    int64_t t1 = time_get_milli_seconds();
    /* mark all objects to be unused */
    for (int i = 0; i < tm->all->len; i++) {
        GC_MARKED(tm->all->nodes[i]) = 0;
    }

    // 标记常量池
    // TODO: 常量可以不进入gc流程
    tm->constants->marked = 0;

    int64_t cost_time = time_get_milli_seconds() - t1;
    log_info("gc_reset_all: cost %lldms", cost_time);
}


void gc_mark_and_check(MpObj obj, const char* source) {
    const char* err = gc_mark_ex(obj, source);
    if (err != NULL) {
        mp_printf("%s:%d/%s failed, obj:%o, source:%s\n", __FILE__, __LINE__, __FUNCTION__, obj, source);
        mp_raise("gc failed: %s", err);
    }
}

void gc_mark_all() {
    int64_t start_time = time_get_milli_seconds();

    /* mark protoes */
    gc_mark_and_check(tm->list_proto, "list_proto");
    gc_mark_and_check(tm->dict_proto, "dict_proto");
    gc_mark_and_check(tm->str_proto, "str_proto");
    gc_mark_and_check(tm->builtins, "builtins");
    gc_mark_and_check(tm->modules, "modules");
    gc_mark_dict(tm->constants);
    gc_mark_and_check(tm->builtins_mod, "builtins_mod");
    
    gc_mark(tm->root);
    gc_mark_frames();
    gc_mark_and_check(tm->ex, "ex");
    gc_mark_and_check(tm->ex_line, "ex_line");

    int64_t cost_time = time_get_milli_seconds() - start_time;
    log_info("gc_mark_all: cost:%lldms", cost_time);
}

/**
 * mark and sweep garbage collection
 *
 * TODO maybe we can mark the object with different value to
 * recognize the GC type of the object.
 */
void gc_full() {
    #ifdef GC_DISABLED
        return;
    #endif

    int64_t t1 = time_get_milli_seconds();

    int old = tm->allocated;
    tm->max_allocated = max(tm->allocated, tm->max_allocated);
    
    // disable gc.
    if (tm->gc_state == 0) {
        return;
    }

    gc_reset_all();

    gc_mark_all();

    // 清理垃圾,这个操作可以增量处理
    gc_sweep();
    tm->gc_threshold = tm->allocated + tm->allocated / 2;
    
    int64_t cost_time = time_get_milli_seconds() - t1;
    log_info("gc_full: old:%d, now:%d, cost:%lldms", old, tm->allocated, cost_time);
}

/**
 * free, free memory
 * release, release the reference of an object
 * destroy, release and free, personal opinion
 */ 
void gc_destroy() {
    MpList* all = tm->all;
    int i;
    
    // MP_TEST
    log_info("gc_destroy: start ...");
    log_info("gc_destroy: max allocated memory: %d K", 
        tm->max_allocated / 1024);
    log_info("gc_destroy: current all->len: %d", tm->all->len);

    if (tm->local_obj_list) {
        log_info("gc_destroy: current local_obj_list->len: %d", 
            tm->local_obj_list->len);
    }
    // MP_TEST_END

    for (i = 0; i < all->len; i++) {
        obj_free(all->nodes[i]);
    }

    if (tm->local_obj_list != NULL) {
        list_free(tm->local_obj_list); // free local_obj_list
    }
    
    list_free(tm->all);

    if (tm->allocated != 0) {
        log_error("***memory leak happens***", tm->allocated);
        log_error("tm->allocated=%d", tm->allocated);
    }
}


/**
 * create new object
 * @param type object type
 * @value object pointer
 */
MpObj obj_new(int type, void* value) {
    MpObj o;
    MP_TYPE(o) = type;
    switch (type) {
    case TYPE_NUM:
        o.value.num = *(double*) value;
        break;
    case TYPE_STR:
        o.value.str = value;
        break;
    case TYPE_LIST:
        GET_LIST(o) = value;
        break;
    case TYPE_DICT:
        GET_DICT(o) = value;
        break;
    case TYPE_MODULE:
        GET_MODULE(o) = value;
        break;
    case TYPE_FUNCTION:
        GET_FUNCTION(o) = value;
        break;
    case TYPE_CLASS:
        GET_CLASS(o) = value;
        break;
    case TYPE_NONE:
        break;
    default:
        mp_raise("obj_new: not supported type %d", type);
    }
    return o;
}

/**
 * free a object
 * @since 2016
 */
void obj_free(MpObj o) {
    switch (MP_TYPE(o)) {
    case TYPE_STR:
        string_free(GET_STR_OBJ(o));
        break;
    case TYPE_LIST:
        list_free(GET_LIST(o));
        break;
    case TYPE_DICT:
        dict_free(GET_DICT(o));
        break;
    case TYPE_FUNCTION:
        func_free(GET_FUNCTION(o));
        break;
    case TYPE_CLASS:
        class_free(GET_CLASS(o));
        break;
    case TYPE_MODULE:
        module_free(o.value.mod);
        break;
    case TYPE_DATA:
        GET_DATA(o)->func_free(GET_DATA(o));
        // GET_DATA_PROTO(o)->free(GET_DATA(o));
        break;
    default:
        mp_raise("gc_free: Unknown type: %d", MP_TYPE(o));
    }
}


MpObj* data_next(MpData* data) {
    mp_raise("data.next not implemented!");
    return NULL;
}

void data_mark(MpData* data) {
    // marked 字段已经在上层处理了
    for (int i = 0; i < data->data_size; i++) {
        gc_mark_ex(data->data_ptr[i], "data");
    }
}

void data_free(MpData* data) {
    // printf("data_free: %x\n", data);
    mp_free(data, sizeof(MpData) + (data->data_size-1) * sizeof(MpObj));
}

MpObj data_get(MpData* data, MpObj key) {
    mp_raise("data.get not implemented");
    return NONE_OBJECT;
}

void data_set(MpData* data, MpObj key, MpObj value) {
    mp_raise("data.set not implemented");
}

MpObj data_str(MpData* data) {
    return string_alloc("data", -1);
}


/** 
 * data_size指的是包含的扩展对象数量(data_ptr数组)
 * data_size is the size of objects contains in the data 
 */
MpData* data_new_ptr(size_t data_size) {
    MpObj data_obj;
    data_obj.type = TYPE_DATA;
    /* there is one slot for default. */
    GET_DATA(data_obj) = mp_malloc(sizeof(MpData) + (data_size-1) * sizeof(MpObj));
    MpData* data = GET_DATA(data_obj);

    data->mark = data_mark;
    data->func_free = data_free;
    data->get  = data_get;
    data->set  = data_set;
    data->next = data_next;
    data->str  = data_str;
    data->data_size = data_size;

    int i = 0;
    for (i = 0; i < data_size; i++) {
        data->data_ptr[i] = NONE_OBJECT;
    }
    gc_track(data_obj);
    return data;
}

MpObj data_ptr_to_obj(MpData* ptr) {
    MpObj result;
    result.type = TYPE_DATA;
    result.value.data = ptr;
    return result;
}

MpObj data_new(size_t data_size) {
    MpData* ptr = data_new_ptr(data_size);
    return data_ptr_to_obj(ptr);
}