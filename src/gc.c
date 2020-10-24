/**
 * 1. mark all allocated object to be unused (0);
 * 2. mark objects can be reached from `root` to be used (1);
 * 3. [at runtime] mark objects can be reached from `frames` to be used (1);
 * 4. [at native call] mark objects in tm->local_obj_list to be used(1);
 * 5. release objects which are marked unused (0).
 * 
 * @since 2015
 * @modified 2020/10/21 01:09:28
 */

#include "include/mp.h"
#include "log.c"

void chars_init();
void frames_init();

#define GC_CONSTANS_LEN 10
#define GC_REACHED_SIGN 1
#define GC_MARKED(o)    (o).value.gc->marked
#define GET_CHAR(n)     ARRAY_CHARS[n]


/**
 * init garbage collector
 * @since 2016
 */
void gc_init() {
    int i;
    
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
    tm->list_proto.type = TYPE_NONE;
    tm->dict_proto.type = TYPE_NONE;
    tm->str_proto.type = TYPE_NONE;
    
    tm->root = list_new(100);
    tm->builtins = dict_new();
    tm->modules = dict_new();
    tm->constants = dict_new();

    /* initialize exception */
    tm->ex_list = list_new(10);
    obj_append(tm->root, tm->ex_list);
    tm->ex = NONE_OBJECT;
    tm->ex_line = NONE_OBJECT;
    
    /* initialize chars */
    chars_init();
    
    /* initialize frames */
    frames_init();
}

void chars_init() {
    int i;
    ARRAY_CHARS = list_new(256); // init global ARRAY_CHARS
    for (i = 0; i < 256; i++) {
        obj_append(ARRAY_CHARS, string_char_new(i));
    }
    obj_append(tm->root, ARRAY_CHARS);
}

void frames_init() {
    int i;
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
    tm->stack_end = tm->stack + STACK_SIZE; // set global mp_stack_end
}


void* mp_malloc(size_t size) {
    void* block;
    Object* func;

    if (size <= 0) {
        mp_raise("mp_malloc, attempts to allocate a memory block of size %d!", size);
        return NULL;
    }
    block = malloc(size);
    
    log_info("malloc,%d,%d,%p", tm->allocated, tm->allocated + size, block);

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

void mp_free(void* o, size_t size) {
    if (o == NULL)
        return;

    log_info("free,%d,%d,%p", tm->allocated, tm->allocated - size, o);
    free(o);
    tm->allocated -= size;
}

/**
 * tracking allocated object
 * @since 2014-??
 * @modified 2016-08-20
 * add to tm->local_obj_list, which will be assumed like frame-locals
 */
Object gc_track(Object v) {
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
            mp_raise("gc_track(), not supported type %d", v.type);
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

void gc_mark_list(MpList* list) {
    if (list->marked)
        return;
    list->marked = GC_REACHED_SIGN;
    int i;
    for (i = 0; i < list->len; i++) {
        gc_mark(list->nodes[i]);
    }
}
 
void gc_mark_dict(MpDict* dict) {
    if (dict->marked)
        return;
    dict->marked = GC_REACHED_SIGN;
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used) {
            gc_mark(dict->nodes[i].key);
            gc_mark(dict->nodes[i].val);
        }
    }
}

void gc_mark_func(MpFunction* func) {
    if (func->marked)
        return;
    func->marked = GC_REACHED_SIGN;
    gc_mark(func->mod);
    gc_mark(func->self);
    gc_mark(func->name);
}

void gc_mark_class(MpClass* pclass) {
    if (pclass->marked) {
        return;
    }

    pclass->marked = GC_REACHED_SIGN;
    gc_mark(pclass->name);
    gc_mark(pclass->attr_dict);
}

/**
 * mark object only once, not cursively
 * @since 2016-08-21
 */
void gc_mark_single(Object o) {
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
    gc_mark(pmodule->code);
    gc_mark(pmodule->file);
    gc_mark(pmodule->globals);
}

/**
 * mark object as used
 * @since 2014-??
 */
void gc_mark(Object o) {
    if (o.type == TYPE_NUM || o.type == TYPE_NONE)
        return;
    switch (o.type) {
    case TYPE_STR: {
        if (o.value.str->marked)
            return;
        o.value.str->marked = GC_REACHED_SIGN;
        break;
    }
    case TYPE_LIST:
        gc_mark_list(GET_LIST(o));
        break;
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
            return;
        GET_DATA(o)->marked = GC_REACHED_SIGN;
        GET_DATA(o)->mark(GET_DATA(o));
        // GET_DATA(o)->proto->mark(GET_DATA(o));
        break;
    default:
        mp_raise("gc_mark(), unknown object type %d", o.type);
    }
}

void gc_unmark(Object o) {
    if (o.type == TYPE_NUM || o.type == TYPE_NONE)
        return;
    GC_MARKED(o) = 0;
}

/**
 * mark objects in frame-local and frame-stack
 * @since 2015-??
 */
void gc_mark_locals_and_stack() {
    MpFrame* f;
    for(f = tm->frames + 1; f <= tm->frame; f++) {
        gc_mark(f->fnc);
        int j;
        /* mark locals */
        for(j = 0; j < f->maxlocals; j++) {
            gc_mark(f->locals[j]);
        }
        /* mark operand stack */
        Object* temp;
        for(temp = f->stack; temp <= f->top; temp++) {
            gc_mark(*temp);
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

    log_info("sweep,%d,0,start", tm->all->len);
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
    
    log_info("deleted_cnt: %d", deleted_cnt);
    log_info("sweep,%d,0,end", tm->all->len);
}

/**
 * mark and sweep garbage collection
 *
 * TODO maybe we can mark the object with different value to
 * recognize the GC type of the object.
 */
void gc_full() {
    int i;
    long t1, t2;
    t1 = clock();

    int old = tm->allocated;
    tm->max_allocated = max(tm->allocated, tm->max_allocated);
    
    // disable gc.
    if (tm->gc_state == 0) {
        return;
    }

    /* mark all objects to be unused */
    for (i = 0; i < tm->all->len; i++) {
        GC_MARKED(tm->all->nodes[i]) = 0;
    }
    
    /* mark protoes */
    gc_mark(tm->list_proto);
    gc_mark(tm->dict_proto);
    gc_mark(tm->str_proto);
    gc_mark(tm->builtins);
    gc_mark(tm->modules);
    gc_mark(tm->constants);
    
    gc_mark(tm->root);
    gc_mark_locals_and_stack();
    gc_mark(tm->ex);
    gc_mark(tm->ex_line);
    
    /* sweep garbage */
    gc_sweep();
    tm->gc_threshold = tm->allocated + tm->allocated / 2;
    t2 = clock();
    
    log_info("full,old:%d,now:%d", old, tm->allocated);
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
    log_info("destroy gc ...");
    log_info("max allocated memory: %d K", tm->max_allocated / 1024);
    log_info("current all->len: %d", tm->all->len);

    if (tm->local_obj_list) {
        log_info("current local_obj_list->len: %d", tm->local_obj_list->len);
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
 * @since ?
 */
Object obj_new(int type, void * value) {
    Object o;
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
 * @since ?
 */
void obj_free(Object o) {
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
    }
}


Object* data_next(MpData* data) {
    mp_raise("next is not defined!");
    return NULL;
}

void data_mark(MpData* data) {
    int i;
    for (i = 0; i < data->data_size; i++) {
        gc_mark(data->data_ptr[i]);
    }
}

void data_free(MpData* data) {
    // printf("data_free: %x\n", data);
    mp_free(data, sizeof(MpData) + (data->data_size-1) * sizeof(Object));
}

Object data_get(Object self, Object key) {
    mp_raise("data.get not implemented");
    return NONE_OBJECT;
}

void data_set(Object self, Object key, Object value) {
    mp_raise("data.set not implemented");
}

Object data_str(Object self) {
    return string_alloc("data", -1);
}


/** 
 * data_size is the size of objects contains in the data 
 */
Object data_new(size_t data_size) {
    Object data_obj;
    data_obj.type = TYPE_DATA;
    /* there is one slot for default. */
    GET_DATA(data_obj) = mp_malloc(sizeof(MpData) + (data_size-1) * sizeof(Object));
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
    return gc_track(data_obj);
}
