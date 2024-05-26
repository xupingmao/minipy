/**
 * less interface is better than more,
 * too many interfaces with similar function will confuse the users.
 * @author xupingmao <578749341@qq.com>
 * @since 2016
 * @modified 2022/06/11 18:41:32
 */
#include "include/mp.h"

static int dict_find_start(MpDict* dict, int hash);
static DictNode* dict_get_node_with_hash(MpDict* dict, MpObj key, int hash);
static void* dict_malloc(MpDict* dict, size_t size, const char* scene);
static void dict_free_block(MpDict* dict, void* block, size_t size);
void dict_free_internal(MpDict* dict);
static MpObj dict_to_obj(MpDict* dict);
static MpDict* dict_new_no_track();



/** 
 *   New hashdict instance, with initial allocated size set to 7.
 *   better not similar to computer binary size ( as to say 2 ), 
 *   such as 2, 10 etc.
 */
static int js_hash(unsigned char* s, int len) {
    int hash = 1315423911;
    int i = 0;
    for(i = 0; i < len; i++)
       hash ^= ((hash << 5) + s[i] + (hash >> 2));
    return abs(hash);
}


int mp_hash(void* s, int len) {
    return js_hash((unsigned char*)s, len);
}

int mp_get_ptr_hash(MpObj* key) {
    switch(key->type) {
        case TYPE_STR:
            return key->value.str->hash;
        case TYPE_NUM:
            return abs((int) key->value.num);
        default: return 0;
    }

    mp_raise("mp_get_obj_hash: unexpected reach");
    return 0;
}

/**
 * simple hash function for dict
 * @since 2015-?
 */
int mp_get_obj_hash(MpObj key) {
    return mp_get_ptr_hash(&key);
}


void dict_reset(MpDict* dict) {
    // to mark that the node is not allocated.
    for(int i = 0; i < dict->cap; i++){
        dict->nodes[i].used = 0;
    }

    for (int i = 0; i < dict->slot_cap; i++) {
        dict->slots[i] = -1;
    }
}

static void* dict_malloc(MpDict* dict, size_t size, const char* scene) {
    if (dict->features & DICT_FEATURE_NO_GC) {
        return malloc(size);
    } else {
        return mp_malloc(size, scene);
    }
}

static void dict_free_block(MpDict* dict, void* block, size_t size) {
    if (dict->features & DICT_FEATURE_NO_GC) {
        assert(block != NULL);
        free(block);
    } else {
        mp_free(block, size);
    }
}

/**
 * init dictionary
 * @since 2015
 */
MpDict* dict_init(MpDict*dict, int cap) {
    int old_cap = dict->cap;
    int old_slot_cap = dict->slot_cap;

    dict->cap = cap;
    dict->slot_cap = cap * 4 / 3 + cap;
    dict->extend = 2;

    dict->nodes = dict_malloc(dict, sizeof(DictNode) * (dict->cap), "dict.init");
    dict->slots = dict_malloc(dict, sizeof(int) * (dict->slot_cap), "dict.init");

    dict->len = 0;
    // 两个的差值就是基数
    dict->mask = (dict->slot_cap) - (dict->cap);
    dict->free_start = 0;
    dict_reset(dict);
    return dict;
}



MpObj dict_new(){
    MpDict* dict = dict_new_no_track();
    MpObj o = dict_to_obj(dict);
    return gc_track(o);
}

MpObj dict_new_obj() {
    return dict_new();
}

MpDict* dict_new_ptr() {
    MpObj obj = dict_new_obj();
    return obj.value.dict;
}

static MpDict* dict_new_no_track() {
    MpDict* dict = mp_malloc(sizeof(MpDict), "dict.new");
    dict->features = 0;
    dict_init(dict, 4);
    return dict;
}

MpDict* dict_new_no_gc() {
    MpDict* dict = malloc(sizeof(MpDict));
    dict->features = 0;
    dict->features = dict->features | DICT_FEATURE_NO_GC;
    dict_init(dict, 4);
    return dict;
}

static MpObj dict_to_obj(MpDict* dict) {
    MpObj o;
    o.type = TYPE_DICT;
    o.value.dict = dict;
    return o;
}

static void dict_check(MpDict* dict){
    if(dict->len < dict->cap) {
        return;
    }

    // printf("dict_check\n");

    int old_cap = dict->cap;
    int old_len = dict->len;
    int old_slot_cap = dict->slot_cap;
    DictNode* old_nodes = dict->nodes;
    int* old_slots = dict->slots;
    
    // 计算新的容量
    int new_cap = 0;
    if (old_cap < 10) {
        new_cap = old_cap + 2;
    } else {
        // osize >= 10
        new_cap += old_cap + old_cap / 2;
    }
    
    dict_init(dict, new_cap);

    for (int i = 0; i < old_cap; i++) {
        DictNode node = old_nodes[i];
        if (node.used > 0) {
            dict_set0(dict, node.key, node.val);
        }
    }

    #ifdef MP_DEBUG
        if (old_len != dict->len) {
            mp_raise("dict_check: old_len=%d, dict->len=%d\n", old_len, dict->len);
        }
    #endif

    dict_free_block(dict, old_nodes, old_cap * sizeof(DictNode));
    dict_free_block(dict, old_slots, old_slot_cap * sizeof(int));
}

/**
 * free dict object
 * @since 2015-?
 */
void dict_free(MpDict* dict){
    if (dict->features & DICT_FEATURE_NO_GC) {
        return;
    }
    dict_free_internal(dict);
}


void dict_free_internal(MpDict* dict){
    PRINT_MP_GC_INFO_START();
    dict_free_block(dict, dict->nodes, (dict->cap) * sizeof(DictNode));
    dict_free_block(dict, dict->slots, (dict->slot_cap) * sizeof(int));
    dict_free_block(dict, dict, sizeof(MpDict));
    PRINT_MP_GC_INFO_END("dict", dict);
}

/**
 * find a free entry to put dict node
 * @since 2015-?
 */
static int findfreepos(MpDict* dict) {
    for(int i = dict->free_start; i < dict->cap; i++) {
        if (dict->nodes[i].used <= 0) {
            return i;
        }
    }

    mp_raise("findfreepos: unexpected reach");
    return -1;
}


// 调试功能
void dict_print_debug_info(MpDict* dict) {
    mp_printf("dict(len=%d, cap=%d, slot_cap=%d, mask=%d)\n", 
        dict->len, dict->cap, dict->slot_cap, dict->mask);

    for (int i = 0; i < dict->slot_cap; i++) {
        int slot = dict->slots[i];
        if (slot < 0) {
            mp_printf("slots[%03d]=%03d -> NULL\n", i, slot);
        } else {
            MpObj key = dict->nodes[slot].key;
            int hash = dict->nodes[slot].hash;
            int start = dict_find_start(dict, hash);
            mp_printf("slots[%03d]=%03d -> %o (%d/%d)\n", i, slot, key, hash, start);
        }
    }
}

/**
 * @return node index
 */
int dict_set0(MpDict* dict, MpObj key, MpObj val){
    int hash = mp_get_ptr_hash(&key);
    DictNode* node = dict_get_node_with_hash(dict, key, hash);
    if (node != NULL) {
        node->val = val;
        return (node - dict->nodes);
    }
    dict_check(dict);

    int pos = findfreepos(dict);
    dict->len++;

    // 插入新数据
    dict->nodes[pos].hash = hash;
    dict->nodes[pos].used = 1;
    dict->nodes[pos].key = key;
    dict->nodes[pos].val = val;

    int start = dict_find_start(dict, hash);
    int is_slot_set = FALSE;
    for (int i = start; i < dict->slot_cap; i++) {
        if (dict->slots[i] < 0) {
            dict->slots[i] = pos;
            is_slot_set = TRUE;
            break;
        }
    }

    if (is_slot_set == FALSE) {
        dict_print_debug_info(dict);
        mp_raise("dict_set0: can not found valid slot! start:%d, cap:%d", start, dict->cap);
    }

    // 更新空闲索引下标
    dict->free_start = pos + 1;
    return pos;
}

/**
 * @since 2016-11-27
 */
int dict_get0(MpDict* dict, MpObj key) {
    DictNode* node = dict_get_node(dict, key);
    if (node != NULL) {
        return node - dict->nodes;
    }
    return -1;
}

static int dict_find_start(MpDict* dict, int hash) {
    // 取模运算只用到了hash的右侧部分的数字
    // 使用&位运算可以充分使用全部的数字，但是要求mask=(2^n-1)
    return hash % dict->mask;
}

DictNode* dict_get_node(MpDict* dict, MpObj key) {
    return dict_get_node_with_hash(dict, key, mp_get_ptr_hash(&key));
}

DictNode* dict_get_node_by_index(MpDict* dict, int index) {
    assert(index >= 0);
    if (index < dict->len) {
        DictNode* node = dict->nodes + index;
        if (node->used > 0) {
            return node;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

static DictNode* dict_get_node_with_hash(MpDict* dict, MpObj key, int hash){
    DictNode* nodes = dict->nodes;
    // 计算开始位置
    int start = dict_find_start(dict, hash);

    // mp_printf("dict_get_node: start=%d, mask=%d, len=%d, cap=%d, key=%o\n", 
    //     start, dict->mask, dict->len, dict->cap, key);

    for (int i = start; i < start + dict->cap; i++) {
        int index = dict->slots[i];

        if (index == -1) {
            // 没有值，退出
            return NULL;
            // continue;
        }

        #ifdef MP_DEBUG
            if (index < 0 || index >= dict->cap) {
                mp_raise("dict_get_node: unexpected index = %d", index);
            }
        #endif

        DictNode* node = nodes+index;

        // 为空或者被删除
        if (node->used <= 0) {
            continue;
        }

        if (hash != node->hash) {
            continue;
        }

        if (mp_is_equals(node->key, key)) {
            // mp_printf("dict_get_node: found node, key=%o, index=%d\n", key, index);
            return nodes + index;
        }
    }
    return NULL;
}

MpObj* dict_get_by_cstr(MpDict* dict, char* key) {
    //int hash = hash_cstr((unsigned char*) key, strlen(key));
    //int idx = hash % dict->cap;
    DictNode* nodes = dict->nodes;
    for (int i = 0; i < dict->cap; i++) {
        // 0:未使用 -1:被删除
        if (nodes[i].used <= 0) {
            continue;
        }

        if (IS_STR(nodes[i].key)
                && strcmp(GET_CSTR(nodes[i].key), key) == 0) {
            return &nodes[i].val;
        }
    } 
    return NULL;
}

void dict_set_by_cstr(MpDict* dict, const char* key, MpObj value) {
    dict_set0(dict, string_from_cstr(key), value);
}

MpObj dict_pop(MpDict* dict, MpObj key, MpObj* _default) {
    DictNode* node = dict_get_node(dict, key);
    if (node == NULL) {
        if (_default != NULL) {
            return *_default;
        }
        mp_raise("dict_pop: key_error %o", key);
    }
    node->used = -1;
    dict->len--;
    int node_index = node - dict->nodes;
    dict->free_start = MIN(dict->free_start, node_index);
    return node->val;
}

void dict_del(MpDict* dict, MpObj key) {
    dict_pop(dict, key, NULL);
}

MpObj dict_keys(MpDict* dict){
    MpObj list = list_new(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used > 0) {
            mp_append(list, dict->nodes[i].key);
        }
    }
    return list;
}

static MpDict* must_get_dict(MpObj obj) {
    if (MP_TYPE(obj) != TYPE_DICT) {
        mp_raise("expect dict but see %ot", obj);
    }
    return obj.value.dict;
}

MpObj* dict_get_by_str(MpObj obj, char* key) {
    return dict_get_by_cstr(must_get_dict(obj), key);
}

MpObj dict_builtin_keys(){
    MpObj dict = mp_take_dict_obj_arg("dict.keys");
    return dict_keys(GET_DICT(dict));
}

MpObj dict_builtin_values() {
    MpObj _d = mp_take_dict_obj_arg("dict.values");
    MpDict* dict = GET_DICT(_d);
    MpObj list = list_new(dict->len);
    for(int i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used > 0) {
            mp_append(list, dict->nodes[i].val);
        }
    }
    return list;
}

MpObj dict_builtin_copy() {
    MpObj dict = mp_take_dict_obj_arg("dict.copy");
    MpObj new_dict = dict_new();

    for(int i = 0; i < GET_DICT(dict)->cap; i++) {
        if (GET_DICT(dict)->nodes[i].used > 0) {
            dict_set0(GET_DICT(new_dict), 
                GET_DICT(dict)->nodes[i].key, 
                GET_DICT(dict)->nodes[i].val);
        }
    }
    return new_dict;
}

MpObj dict_builtin_update() {
    MpDict* self = mp_take_dict_ptr_arg("dict.update");
    MpDict* other = mp_take_dict_ptr_arg("dict.update");

    for (int i = 0; i < other->cap; i++) {
        if (other->nodes[i].used <= 0) {
            continue;
        }
        dict_set0(self, other->nodes[i].key, 
            other->nodes[i].val);
    }
    return dict_to_obj(self);
}

MpObj dict_builtin_pop() {
    MpObj self = mp_take_dict_obj_arg("dict.pop");
    MpObj key  = mp_take_obj_arg("dict.pop");
    return dict_pop(GET_DICT(self), key, NULL);
}

MpObj dict_builtin_get() {
    MpDict* self = mp_take_dict_ptr_arg("dict.get");
    MpObj key = mp_take_obj_arg("dict.get");
    DictNode* node = dict_get_node(self, key);
    if (node == NULL) {
        return NONE_OBJECT;
    }
    return node->val;
}

/**
 * init dict methods
 * @since 2015-?
 */
void MpDict_InitMethods() {
    tm->dict_proto = dict_new();
    /* build dict class */
    MpModule_RegFunc(tm->dict_proto, "get",    dict_builtin_get);
    MpModule_RegFunc(tm->dict_proto, "keys",   dict_builtin_keys);
    MpModule_RegFunc(tm->dict_proto, "values", dict_builtin_values);
    MpModule_RegFunc(tm->dict_proto, "copy",   dict_builtin_copy);
    MpModule_RegFunc(tm->dict_proto, "update", dict_builtin_update);
    MpModule_RegFunc(tm->dict_proto, "pop",    dict_builtin_pop);
}

/**
 * create a dict iterator from dict object
 * @since 2016-?
 */
MpObj dict_iter_new(MpObj dict) {
    mp_assert_type(dict, TYPE_DICT, "dict_iter_new");
    MpObj data = data_new(1);
    MpData* iterator = GET_DATA(data);
    iterator->cur = 0;
    iterator->end = DICT_LEN(dict);
    iterator->inc = 1;
    iterator->data_ptr[0] = dict;
    iterator->next = dict_next;
    return data;
}

/**
 * get next node pointer
 * @since 2015-?
 */
MpObj* dict_next(MpData* iterator) {
    mp_assert_type(iterator->data_ptr[0], TYPE_DICT, "dict_next");
    MpDict* dict = GET_DICT(iterator->data_ptr[0]);
    if (iterator->cur < dict->cap) {
        for(int i = iterator->cur; i < dict->cap; i++) {
            if (dict->nodes[i].used > 0) {
                iterator->cur = i + 1;
                return &dict->nodes[i].key;
            }
        }
        return NULL;
    }
    return NULL;
}


/**
 * get attribute by char array
 * @since 2016-09-02
 */
MpObj mp_getattr(MpObj obj, const char* key) {
    // TODO optimize string find
    MpObj obj_key = string_static(key);
    return obj_get(obj, obj_key);
}


/**
 * get attribute by char array
 * @since 2016-09-02
 */
void mp_setattr(MpObj obj, const char* key, MpObj value) {
    MpObj obj_key = string_static(key);
    obj_set(obj, obj_key, value);
}

int mp_hasattr(MpObj obj, const char* key) {
    mp_assert_type(obj, TYPE_DICT, "mp_hasattr");

    MpObj obj_key = string_static(key);
    DictNode* node = dict_get_node(GET_DICT(obj), obj_key);
    if (node == NULL) {
        return 0;
    }
    return 1;
}

size_t dict_sizeof(MpDict* dict) {
    size_t result = 0;
    result += sizeof(MpDict);
    result += sizeof(int) * dict->slot_cap;
    result += sizeof(DictNode) * dict->cap;
    return result;
}
