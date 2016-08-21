#include "include/tm.h"

#ifdef GC_DEBUG
    #include "debug.c"
#endif

/**
 * 1. mark all allocated object to be unused (0);
 * 2. mark objects can be reached from `root` to be used (1);
 * 3. mark objects can be reached from `frames` to be used (1);
 * 4. release objects which are marked unused (0).
 * 
 */



enum {
    GC_INIT,
    GC_WORKING,
    GC_STATE_DESTROY
} GcState;

#ifdef GC_DEBUG
// logs can redirected to file
#define WRITE_LOG_FILE 0

static char LOG_BUF[1024 * 10]; // 20K buffer

enum {
    LEVEL_DEBUG,
    LEVEL_INFO, 
    LEVEL_ERROR,
    LEVEL_FATAL,
} LogLevel;

// debug, info, error, fatal
static int gc_log_level = LEVEL_ERROR;
static long long gc_log_count = 0;


int check_log_level(int level) {
    if (gc_log_level == LEVEL_FATAL) {
        return 1;
    } else if (gc_log_level == LEVEL_ERROR) {
        return level == LEVEL_ERROR || level == LEVEL_FATAL;
    } else if (gc_log_level == LEVEL_INFO) {
        return level != LEVEL_DEBUG;
    } else if (gc_log_level == LEVEL_DEBUG) {
        return level == LEVEL_DEBUG;
    }
    return 0;
}

void DEBUG_INIT() {
    if (WRITE_LOG_FILE) {
        FILE* fp = fopen("tm.log", "w");
        // fputs("*****LOG INIT*****\n", fp);
        fclose(fp);
    }

    /** init debug map */
    debug_init();
}

void DEBUG_MALLOC(void* ptr) {
    debug_malloc(ptr);
}

void DEBUG_FREE(void*ptr) {
    debug_free(ptr);
}

void DEBUG_FREE2(void*ptr, Object o) {
    debug_free2(ptr, o);
}

 /**
  * logging service
  * suggested format
  * TIME|OPT|CURRENT_SIZE|NEW_SIZE|ADDR
  */
void LOG(int level, char* fmt, ...) {
    va_list ap;
    time_t cur_time;
    char *buffer   = LOG_BUF;
    int BUF_SIZE   = sizeof(LOG_BUF);
    int FLUSH_SIZE = BUF_SIZE / 2;
    char temp_buf[1024];

    if (!check_log_level(level)) {
        return;
    }

    memset(temp_buf, 0, sizeof(temp_buf));
    va_start(ap, fmt);

    time(&cur_time); // init time
    char* t_str   = ctime(&cur_time);
    int t_str_len = strlen(t_str);
    t_str[t_str_len-1] = 0; // remove \n

    sprintf (temp_buf, "%s,", t_str); // print time
    strcat(buffer, temp_buf);

    vsprintf(temp_buf, fmt, ap);

    char count_buf[20];
    sprintf(count_buf, ",%d", gc_log_count);
    gc_log_count += 1;

    strcat(temp_buf, count_buf);
    strcat(temp_buf, "\n");

    fputs(temp_buf, stdout); // print to stdout

    strcat(buffer, temp_buf);

    va_end(ap);

    if (WRITE_LOG_FILE && strlen(buffer) >= FLUSH_SIZE) {
        FILE* fp = fopen("tm.log", "a");
        fputs(buffer, fp);
        fclose(fp);
        memset(buffer, 0, BUF_SIZE);
    }
}

/**
 * flush log
 * @since 2016-08-16
 */
void LOG_END() {

    if (WRITE_LOG_FILE) {
        FILE* fp = fopen("tm.log", "a");
        fputs(LOG_BUF, fp);
        fclose(fp);
        memset(LOG_BUF, 0, sizeof(LOG_BUF));
    }

    debug_destroy();
}
#else
    #define DEBUG_INIT() /* DEBUG_INIT */
    #define LOG(level, fmt, oldsize, newsize, addr) /* LOG */
    #define LOG_END() /* LOG END */
    #define DEBUG_MALLOC(ptr) /* DEBUG_MALLOC */
    #define DEBUG_FREE(ptr) /* DEBUG_FREE */
    #define DEBUG_FREE2(ptr,o) /* DEBUG_FREE2 */
#endif

void chars_init();
void frames_init();

#define GC_CONSTANS_LEN 10
#define GC_REACHED_SIGN 1
#define GC_MARKED(o) (o).value.gc->marked

#define GET_CHAR(n) ARRAY_CHARS[n]


/**
 * init garbage collector
 * @since ?
 */
void gc_init() {
    int i;
    
    /* initialize constants */
    NUMBER_TRUE = tm_number(1);
    NUMBER_FALSE = tm_number(0);
    NONE_OBJECT.type = TYPE_NONE;
    UNDEF.type = -1;
    
    tm->init = 0;
    tm->debug = 0;
    tm->allocated = 0;
    tm->max_allocated = 0;
    tm->gc_threshold = 1024 * 8; // set 8k to see gc process
    tm->gc_state = 1; // enable gc.

    tm->all = untracked_list_new(100);
    tm->local_obj_list = NULL; // object allocated in local scope. which can be sweeped simply.
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

    DEBUG_INIT();
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
        TmFrame* f = tm->frames + i;
        f->stack = f->top = tm->stack;
        f->lineno = -1;
        f->fnc = NONE_OBJECT;
        f->pc = NULL;
        f->maxlocals = 0;
        f->maxstack = 0;
        f->jmp = NULL;
        f->idx = i;
    }
    tm->frames_init_done = 1;
    tm->frame = tm->frames;
    tm_stack_end = tm->stack + STACK_SIZE; // set global tm_stack_end
}


void* tm_malloc(size_t size) {
    void* block;
    Object* func;

    if (size <= 0) {
        tm_raise("tm_malloc, attempts to allocate a memory block of size %d!", size);
        return NULL;
    }
    block = malloc(size);
    
    DEBUG_MALLOC(block);
    LOG(LEVEL_INFO,"malloc,%d,%d,%p", tm->allocated, tm->allocated + size, block);

    if (block == NULL) {
        tm_raise("tm_malloc: fail to malloc memory block of size %d", size);
    }
    tm->allocated += size;

#if !PRODUCT
    tm->max_allocated = max(tm->max_allocated, tm->allocated);
#endif

    return block;
}

void* tm_realloc(void* o, size_t osize, size_t nsize) {
    void* block = tm_malloc(nsize);
    memcpy(block, o, osize);
    tm_free(o, osize);
    return block;
}

void tm_free(void* o, size_t size) {
    if (o == NULL)
        return;

    LOG(LEVEL_INFO, "free,%d,%d,%p", tm->allocated, tm->allocated - size, o);
    free(o);
    tm->allocated -= size;
}

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
    default:
        tm_raise("gc_track(), not supported type %d", v.type);
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

/*
 not implemented yet.
 
PtrArray* gc_ptr_array_new() {
    PtrArray* arr = tm_malloc(sizeof(PtrArray));
    arr->cap = 100;
    arr->len = 0;
    arr->elements = tm_malloc(sizeof(Object*) * arr->cap);
    return arr;
}

void gc_ptr_array_pop(PtrArray* arr) {
    if (arr->len <= 0) return;
    arr->elements[arr->len-1] = NULL;
    arr->len -= 1;
}

void gc_ptr_array_check(PtrArray* arr) {
    if (arr->len == arr->cap) {
        int old_cap = arr->cap;
        arr->cap += arr->cap / 2 + 1;
        arr->elements = tm_realloc(arr->elements, sizeof(Object*) * old_cap, sizeof(Object*) * arr->cap);
    }
}

void gc_ptr_array_append(PtrArray* arr, Object* obj) {
    gc_ptr_array_check(arr);
    arr->elements[arr->len] = obj;
    arr->len += 1;
}

void gc_ptr_array_free(PtrArray* arr) {
    tm_free(arr->elements, sizeof(Object*) * arr->cap);
    tm_free(arr, sizeof(PtrArray));
}

void gc_track_local(Object* local) {
    // gc_ptr_array_append(tm->local_obj_list, local);
}

void gc_pop_local() {
    // gc_ptr_array_pop(tm->local_obj_list);
}

void gc_pop_locals(int n) {
    while (--n >= 0) {
        gc_pop_local();
    }
}

*/
void gc_mark_list(TmList* list) {
    if (list->marked)
        return;
    list->marked = GC_REACHED_SIGN;
    int i;
    for (i = 0; i < list->len; i++) {
        gc_mark(list->nodes[i]);
    }
}
 
void gc_mark_dict(TmDict* dict) {
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

void gc_mark_func(TmFunction* func) {
    if (func->marked)
        return;
    func->marked = GC_REACHED_SIGN;
    gc_mark(func->mod);
    gc_mark(func->self);
    gc_mark(func->name);
}

void gc_mark_single(Object o) {
    if (o.type == TYPE_NUM || o.type == TYPE_NONE) {
        return;
    }
    GC_MARKED(o) = 1;
}

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
    case TYPE_MODULE:
        if (GET_MODULE(o)->marked)
            return;
        GET_MODULE(o)->marked = GC_REACHED_SIGN;
        gc_mark(GET_MODULE(o)->code);
        gc_mark(GET_MODULE(o)->file);
        /*gc_mark(GET_MODULE(o)->constants);*/
        gc_mark(GET_MODULE(o)->globals);
        break;
    case TYPE_DATA:
        if (GET_DATA(o)->marked)
            return;
        GET_DATA(o)->marked = GC_REACHED_SIGN;
        GET_DATA(o)->mark(GET_DATA(o));
        // GET_DATA(o)->proto->mark(GET_DATA(o));
        break;
    default:
        tm_raise("gc_mark(), unknown object type %d", o.type);
    }
}

void gc_unmark(Object o) {
    if (o.type == TYPE_NUM || o.type == TYPE_NONE)
        return;
    GC_MARKED(o) = 0;
}

void gc_mark_locals_and_stack() {
    TmFrame* f;
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
 * @since ?
 * @modified 2016-08-20
 */
void gc_sweep() {
    int n, i;

    LOG(LEVEL_ERROR, "sweep,%d,0,start", tm->all->len, 0, 0);
    TmList* temp = untracked_list_new(200);
    TmList* all = tm->all;

    for (i = 0; i < all->len; i++) {
        if (GC_MARKED(tm->all->nodes[i])) {
            list_append(temp, all->nodes[i]);
        } else {
            obj_free(all->nodes[i]);
            // list_append(temp, all->nodes[i]);
        }
    }
    list_free(tm->all);
    tm->all = temp;

    LOG(LEVEL_ERROR, "sweep,%d,0,end", tm->all->len, 0,0);
}

/**
 * @deprecated
 * sweep object not used in frame-local
 * <code>
 *    z = new string('test');
 *    k = new string('kkkk');
 *    return k; // z will be deleted
 * </code>
 * @since 2016-08-19
 */
void gc_sweep_local(int start) {
    if (tm->local_obj_list != NULL) {
        int i;
        TmList* list = tm->local_obj_list;
        for (i = start; i < list->len; i++) {
            if (GC_MARKED(list->nodes[i])==0) {
                obj_free(list->nodes[i]);
            } else {
                list_append(tm->all, list->nodes[i]); // monitor object which cant be recycled
            }
        }
    }
}

/**
 * restore tm->local_obj_list to `size`
 * move extra objects to tm->all
 * @since 2016-08-19
 */
void gc_restore_local_obj_list(int size) {
    int i;
    /*
    for (i = size; i < tm->local_obj_list->len; i++) {
        Object obj = tm->local_obj_list->nodes[i];
        list_append(tm->all, obj); // move objects in current frame to tm->all
    }
    */
    list_shorten(tm->local_obj_list, size);
}

Object get_tm_local_list() {
    Object obj;
    TM_TYPE(obj) = TYPE_LIST;
    GET_LIST(obj) = tm->local_obj_list;
    return obj;
}

/**
 * sweep garbage after native function call is ended.
 * @author xupingmao
 * @since 2016-08-19
 */
void gc_native_call_sweep(Object ret) {
    int i;
    long t1, t2;
    t1 = clock();
#if GC_DEBUG
    int old = tm->allocated;
#endif
    tm->max_allocated = max(tm->allocated, tm->max_allocated);
    LOG(LEVEL_ERROR, "nativefull,%d,%d,start", tm->allocated, tm->max_allocated, 0);
    LOG(LEVEL_ERROR, "all_len,%d,%d,%d", tm->all->len, 0, 0);
    LOG(LEVEL_ERROR, "local_len,%d,%d,%d", tm->local_obj_list->len, 0,0);
    
    /* mark all objects to be unused */
    /* mark tm->all is enough */
    for (i = 0; i < tm->all->len; i++) {
        GC_MARKED(tm->all->nodes[i]) = 0;
    }

    if (TM_TYPE(ret) != TYPE_NONE && TM_TYPE(ret) != TYPE_NUM) {
        // arguments is already in prev-call-stack
        // ret must be added to prev-call-stack
        list_append(tm->local_obj_list, ret); // move ret value to tm->local_obj_list
    }
    // tm_print(get_tm_local_list());

    /* mark protoes */
    gc_mark(tm->list_proto);
    gc_mark(tm->dict_proto);
    gc_mark(tm->str_proto);
    gc_mark(tm->builtins);
    gc_mark(tm->modules);
    gc_mark(tm->constants);
    
    gc_mark(tm->root);
    gc_mark(tm->ex);
    gc_mark(tm->ex_line);
    
    tm->local_obj_list->marked = 0;
    gc_mark_list(tm->local_obj_list);
    /* sweep garbage */
    gc_sweep();
    tm->gc_threshold = tm->allocated + tm->allocated / 2;
    t2 = clock();

    LOG(LEVEL_ERROR, "nativefull,%d,%d,end", old, tm->allocated, "null");
    LOG(LEVEL_ERROR, "all_len,%d,%d,%d", tm->all->len, 0, 0);
    LOG(LEVEL_ERROR, "local_len,%d,%d,%d", tm->local_obj_list->len, 0,0);
}

#define MARK(v) \
    switch( v.type ){  \
    case TYPE_STR: \
        v.value.str->marked = 0;\
        break;\
    case TYPE_LIST:\
        GET_LIST(v)->marked = 0;\
        break;\
    case TYPE_DICT:\
        GET_DICT(v)->marked = 0;\
        break;\
    case TM_MOD:\
        GET_MODULE(v)->marked = 0;\
        break;\
    case TYPE_FUNCTION:\
        GET_FUNCTION(v)->marked = 0;\
        break;\
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
#if GC_DEBUG
    int old = tm->allocated;
#endif
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

    LOG(LEVEL_ERROR, "full,%d,%d,%s", old, tm->allocated, "null");
}

int gc_contains(TmList* list, Object obj) {
    int i;
    for (i = 0; i < list->len; i++) {
        if (GET_PTR(list->nodes[i]) == GET_PTR(obj)) {
            return 1;
        }
    }
    return 0;
}

/**
 * move local object to tm->all
 * note that there may be duplication in local_obj_list
 * @since 2016-08-20
 */
void gc_move_local() {
    if (tm->local_obj_list == NULL) {
        return;
    }
    int i;
    for (i = 0; i < tm->local_obj_list->len; i++) {
        Object obj = tm->local_obj_list->nodes[i];
        if (!gc_contains(tm->all, obj)) {
            list_append(tm->all, obj);
        }
    }
}

/**
 * free, free memory
 * release, release the reference of an object
 * destroy, release and free, personal opinion
 */ 
void gc_destroy() {
    TmList* all = tm->all;
    int i;

    tm->gc_state = GC_STATE_DESTROY; // end

    for (i = 0; i < all->len; i++) {
        obj_free(all->nodes[i]);
    }

    if (tm->local_obj_list != NULL) {
        list_free(tm->local_obj_list); // free local_obj_list
    }
    
    list_free(tm->all);

#if !PRODUCT
    if (tm->allocated != 0) {
        printf("\n***memory leak happens***\ntm->allocated=%d\n", tm->allocated);
    }
#endif
    LOG_END(); // flush the LOG_BUF to file.
}


/**
 * create new object
 * @param type object type
 * @value object pointer
 * @since ?
 */
Object obj_new(int type, void * value) {
    Object o;
    TM_TYPE(o) = type;
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
    case TYPE_NONE:
        break;
    default:
        tm_raise("obj_new: not supported type %d", type);
    }
    return o;
}

/**
 * free a object
 * @since ?
 */
void obj_free(Object o) {

    // LOG(LEVEL_ERROR, "delete,start,%d,%d", TM_TYPE(o), 0);

    #ifdef GC_DEBUG
        if (tm->gc_state != GC_STATE_DESTROY){
            switch(TM_TYPE(o)) {
                case TYPE_STR:  LOG(LEVEL_INFO, "delete,str,%d,%p",    GET_STR_LEN(o), GET_STR_OBJ(o)); break;
                case TYPE_LIST: LOG(LEVEL_INFO, "delete,list,%d,%p",   LIST_LEN(o), GET_LIST(o));    break;
                case TYPE_DICT: LOG(LEVEL_INFO, "delete,dict,%d,%p",   DICT_LEN(o), GET_DICT(o));    break;
                case TYPE_FUNCTION: LOG(LEVEL_INFO, "delete,function,0,%p", GET_FUNCTION(o), 0);     break;
                default:        LOG(LEVEL_INFO, "delete,unknown,%d,%d", TM_TYPE(o), 0); break;
            }
        }

        DEBUG_FREE2(GET_PTR(o), o);
    #endif

    switch (TM_TYPE(o)) {
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
    case TYPE_MODULE:
        module_free(o.value.mod);
        break;
    case TYPE_DATA:
        GET_DATA(o)->free(GET_DATA(o));
        // GET_DATA_PROTO(o)->free(GET_DATA(o));
        break;
    }
}


Object* data_next(TmData* data) {
    tm_raise("next is not defined!");
    return NULL;
}

Data_proto* get_default_data_proto() {
    if(!default_data_proto.init) {
        default_data_proto.init = 1;
        default_data_proto.mark = data_mark;
        default_data_proto.free = data_free;
        default_data_proto.next = data_next;
        default_data_proto.get = data_get;
        default_data_proto.set = data_set;
        default_data_proto.str = data_str;
        default_data_proto.data_size = sizeof(Data_proto);
    }
    return &default_data_proto;
}


void data_mark(TmData* data) {
    int i;
    for (i = 0; i < data->data_size; i++) {
        gc_mark(data->data_ptr[i]);
    }
}

void data_free(TmData* data) {
    // printf("data_free: %x\n", data);
    tm_free(data, sizeof(TmData) + (data->data_size-1) * sizeof(Object));
}

Object data_get(Object self, Object key) {
    tm_raise("data.get not implemented");
    return NONE_OBJECT;
}

void data_set(Object self, Object key, Object value) {
    tm_raise("data.set not implemented");
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
    GET_DATA(data_obj) = tm_malloc(sizeof(TmData) + (data_size-1) * sizeof(Object));
    TmData* data = GET_DATA(data_obj);

    // printf("data_new: %x\n", data);

/*    GET_DATA_PROTO(data)->next = data_next;
    GET_DATA_PROTO(data)->mark = data_mark;
    GET_DATA_PROTO(data)->free = data_free;
    GET_DATA_PROTO(data)->get = data_get;
    GET_DATA_PROTO(data)->set = data_set;
    GET_DATA_PROTO(data) = get_default_data_proto();
*/
    data->mark = data_mark;
    data->free = data_free;
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
