/**
 * less interface is better than more,
 * too many interfaces with similar function will confuse the users.
 * @author xupingmao <578749341@qq.com>
 * @since 2016
 * @modified 2022/01/19 00:52:24
 */
#include "include/mp.h"

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

static int lua_hash(void const *v,int l) {
    int i,step = (l>>5)+1;
    int h = l + (l >= 4?*(int*)v:0);
    for (i=l; i>=step; i-=step) {
        h = h^((h<<5)+(h>>2)+((unsigned char *)v)[i-1]);
    }
    return h;
}

int mp_hash(void* s, int len) {
    return js_hash((unsigned char*)s, len);
}

/**
 * simple hash function for dict
 * @since 2015-?
 */
static int obj_hash(MpObj key) {
    switch(MP_TYPE(key)) {
    case TYPE_STR: {
        return GET_STR_OBJ(key)->hash;
    }
    case TYPE_NUM:return abs((int) GET_NUM(key));
    default: return 0;
    }
    return 0;
}

/**
 * init dictionary
 * @since 2015-?
 */
MpDict* dict_init(){
    MpDict * dict = mp_malloc(sizeof(MpDict));
    dict->cap = 4;
    dict->extend = 2;
    dict->nodes = mp_malloc(sizeof(DictNode) * (dict->cap));
    dict->slots  = mp_malloc(sizeof(HashSlot) * (dict->cap));
    dict->others = mp_malloc(sizeof(int) * (dict->cap));

    dict->len = 0;
    dict->mask = dict->cap - 1;

    // 初始化数据
    // to mark that the node is not allocated.
    for(int i = 0; i < dict->cap; i++){
        // 重置节点
        dict->nodes[i].used = 0;
        dict->nodes[i].hash = 0;

        // 重置索引数据
        for (int j = 0; j < DICT_ZIP_SIZE; j++) {
            dict->slots[i].index[j] = -1;
        }
        // 冲突链表
        dict->others[i] = -1;
    }
    return dict;
}

MpObj dict_new(){
    MpObj o;
    o.type = TYPE_DICT;
    GET_DICT(o) = dict_init();
    return gc_track(o);
}

MpObj dict_new_obj() {
    return dict_new();
}

static
void dict_check(MpDict* dict){
    if(dict->len < dict->cap)
        return;

    int old_cap = dict->cap;
    int nsize = 0;
    
    if (old_cap < 10) {
        // 小字典
        nsize = old_cap + 2;
    } else {
        nsize = old_cap + old_cap / 2 + 1;
    }
    
    DictNode* nodes = mp_malloc(nsize * sizeof(DictNode));
    HashSlot* slots = mp_malloc(nsize * sizeof(HashSlot));
    int* others = mp_malloc(nsize * sizeof(int));
    int mask = nsize - 1;

    for(int i = 0; i < nsize; i++) {
        nodes[i].used = 0;
        for (int j = 0; j < DICT_ZIP_SIZE; j++) {
            slots[i].index[j] = -1;
        }
        others[i] = -1;
    }

    int j = 0;
    for(int i = 0; i < old_cap; i++) {
        if (dict->nodes[i].used > 0) {
            int idx = j; // 新的索引下标
            nodes[j] = dict->nodes[i];
            j++;

            int hash = dict->nodes[i].hash;
            int p = hash & mask;
            HashSlot slot = slots[p];
            int index_handled = 0;
            for (int k = 0; k < DICT_ZIP_SIZE; k++) {
                if (slot.index[k] < 0) {
                    slot.index[k] = idx;
                    index_handled = 1;
                    break;
                }
            }

            // 索引满了，放到冲突序列里面
            if (index_handled == 0) {
                for (int k = 0; k < nsize; k++) {
                    if (others[k] < 0) {
                        others[k] = idx;
                        break;
                    }
                }
            }
        }
    }

    DictNode* temp = dict->nodes;
    HashSlot* old_slots = dict->slots;
    int* old_others = dict->others;

    dict->nodes = nodes;
    dict->slots = slots;
    dict->others = others;
    dict->cap = nsize;
    // 最好是 2^n - 1
    dict->mask = mask;
    mp_free(temp, old_cap * sizeof(DictNode));
    mp_free(old_slots, old_cap * sizeof(HashSlot));
    mp_free(old_others, old_cap * sizeof(int));
}

/**
 * free dict object
 * @since 2015-?
 */
void dict_free(MpDict* dict){
    PRINT_OBJ_GC_INFO_START();
    mp_free(dict->nodes, (dict->cap) * sizeof(DictNode));
    mp_free(dict->slots, (dict->cap) * sizeof(HashSlot));
    mp_free(dict->others, (dict->cap) * sizeof(int));
    mp_free(dict, sizeof(MpDict));
    PRINT_OBJ_GC_INFO_END("dict", dict);
}

/**
 * find a free entry to put dict node
 * @since 2015-?
 */
static int findfreepos(MpDict* dict, MpObj key) {
    // 先找一个空节点
    for(int i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used <= 0) {
            return i;
        }
    }

    mp_raise("findfreepos: unexpected reach");
    return -1;
}

static int findfreepos_new(MpDict* dict, MpObj key) {
    int hash = obj_hash(key);
    int p = hash & dict->mask;

    HashSlot slot = dict->slots[p];
    int found = -1;

    // 先找一个空节点
    for(int i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used == 0) {
            found = i;
        }
    }

    assert(found >= 0);

    // 更新索引
    for (int i = 0; i < DICT_ZIP_SIZE; i++) {
        int idx = slot.index[i];
        if (idx < 0) {
            slot.index[i] = found;
            return found;
        }
    }

    // 没找到
    for (int i = 0; i < dict->cap; i++) {
        if (dict->others[i] < 0) {
            dict->others[i] = found;
            return found;
        }
    }

    mp_raise("findfreepos: unexpected reach");
    return -1;
}

/**
 * @return node index
 */
int dict_set0(MpDict* dict, MpObj key, MpObj val){
    int i;
    DictNode* node = dict_get_node(dict, key);
    if (node != NULL) {
        node->val = val;
        return (node - dict->nodes);
    }
    dict_check(dict);
    i = findfreepos(dict, key);
    dict->len++;
    dict->nodes[i].used = 1;
    dict->nodes[i].key = key;
    dict->nodes[i].val = val;
    dict->nodes[i].hash = obj_hash(key);
    return i;
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

int dict_set_attr(MpDict* dict, int const_id, MpObj val) {
    int i;
    DictNode* nodes = dict->nodes;
    /* start from 2, as 0,1 are used by normal node. */
    const_id += 2; 
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used == const_id) {
            nodes[i].val = val;
            return i;
        }
    }
    MpObj key = GET_CONST(const_id-2);
    i = dict_set0(dict, key, val);
    dict->nodes[i].used = const_id;
    return i;
}

int dict_get_attr(MpDict* dict, int const_id) {
    int i = 0;

    DictNode* nodes = dict->nodes;
    /* prevent first const to be 0, and normal dict node to be 1. */
    const_id += 2; 
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used == const_id) {
            return i;
        }
    }
    DictNode* node = dict_get_node(dict, GET_CONST(const_id-2));
    if (node != NULL) {
        node->used = const_id;
        return node - nodes;
    }
    return -1;
}

DictNode* dict_get_node(MpDict* dict, MpObj key){
    DictNode* nodes = dict->nodes;
    int hash = obj_hash(key);

    for (int i = 0; i < dict->cap; i++) {
        if (nodes[i].used > 0 && nodes[i].hash == hash 
                && is_obj_equals(nodes[i].key, key)) {
            return nodes + i;
        }
    }

    return NULL;
}

DictNode* dict_get_node_new(MpDict* dict, MpObj key){
    int i = 0;
    int hash = obj_hash(key);

    // 先在索引中搜索
    HashSlot slot = dict->slots[hash & dict->mask];
    for (int i = 0; i < DICT_ZIP_SIZE; i++) {
        int index = slot.index[i];
        // 不存在
        if (index == -1) {
            return NULL;
        }

        // 被删除的节点索引
        if (index == -2) {
            continue;
        }
        DictNode node = dict->nodes[index];
        if (node.used > 0 && is_obj_equals(node.key, key)) {
            return dict->nodes + index;
        }
    }
    
    // 没找到，到冲突链表上找
    for (int i = 0; i < dict->len; i++) {
        int index = dict->others[i];
        if (index < 0) {
            continue;
        }
        DictNode node = dict->nodes[index];
        if (node.used > 0 && is_obj_equals(node.key, key)) {
            return dict->nodes + index;
        }
    }

    return NULL;
}

MpObj* dict_get_by_cstr(MpDict* dict, char* key) {
    //int hash = hash_cstr((unsigned char*) key, strlen(key));
    //int idx = hash % dict->cap;
    int i = 0;
    DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used > 0 && IS_STR(nodes[i].key)
                && strcmp(GET_CSTR(nodes[i].key), key) == 0) {
            return &nodes[i].val;
        }
    } 
    return NULL;
}

void dict_set_by_cstr(MpDict* dict, char* key, MpObj value) {
    dict_set0(dict, string_from_cstr(key), value);
}

void dict_del(MpDict* dict, MpObj key) {
    DictNode* node = dict_get_node(dict, key);
    if (node == NULL) {
        mp_raise("obj_del: key_error %o", key);
    }

    // 被删除需要标记为-1
    node->used = -1;
    dict->len--;
    return;
}

MpObj dict_keys(MpDict* dict){
    MpObj list = list_new(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used > 0) {
            obj_append(list, dict->nodes[i].key);
        }
    }
    return list;
}

MpObj dict_builtin_keys(){
    MpObj dict = arg_take_dict_obj("dict.keys");
    return dict_keys(GET_DICT(dict));
}

MpObj dict_builtin_values() {
    MpObj _d = arg_take_dict_obj("dict.values");
    MpDict* dict = GET_DICT(_d);
    MpObj list = list_new(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used) {
            obj_append(list, dict->nodes[i].val);
        }
    }
    return list;
}

MpObj dict_builtin_copy() {
    MpObj dict = arg_take_dict_obj("dict.copy");
    MpObj new_dict = dict_new();
    int i;
    for(i = 0; i < GET_DICT(dict)->cap; i++) {
        if (GET_DICT(dict)->nodes[i].used > 0) {
            dict_set0(GET_DICT(new_dict), GET_DICT(dict)->nodes[i].key, GET_DICT(dict)->nodes[i].val);
        }
    }
    return new_dict;
}

MpObj dict_builtin_update() {
    MpObj self = arg_take_dict_obj("dict.update");
    MpObj other = arg_take_dict_obj("dict.update");
    int i = 0;

    for (i = 0; i < DICT_LEN(other); i++) {
        dict_set0(GET_DICT(self), GET_DICT(other)->nodes[i].key, GET_DICT(other)->nodes[i].val);
    }
    return self;
}

MpObj dict_builtin_pop() {
    MpObj self = arg_take_dict_obj("dict.pop");
    MpObj key  = arg_take_obj("dict.pop");
    DictNode* node = dict_get_node(GET_DICT(self), key);
    if (node == NULL) {    
        if (arg_has_next()) {
            return arg_take_obj("dict.pop");
        } else {
            mp_raise("KeyError %o", key);
            return NONE_OBJECT;
        }
    } else {
        node->used = 0;
        return node->val;
    }
}

/**
 * init dict methods
 * @since 2015-?
 */
void dict_methods_init() {
    tm->dict_proto = dict_new();
    /* build dict class */
    reg_mod_func(tm->dict_proto, "keys",   dict_builtin_keys);
    reg_mod_func(tm->dict_proto, "values", dict_builtin_values);
    reg_mod_func(tm->dict_proto, "copy",   dict_builtin_copy);
    reg_mod_func(tm->dict_proto, "update", dict_builtin_update);
    reg_mod_func(tm->dict_proto, "pop",    dict_builtin_pop);
}

/**
 * create a dict iterator from dict object
 * @since 2016-?
 */
MpObj dict_iter_new(MpObj dict) {
    /*
    MpObj *__iter__ = dict_get_by_str(dict, "__iter__");
    if (__iter__ != NULL) {
        MpObj *next = dict_get_by_str(dict, "next");
        MpObj data = data_new(sizeof(Mp_base_iterator));
        Mp_base_iterator* base_iterator = (Mp_base_iterator*)GET_DATA(data);
        base_iterator->func = *next;
        base_iterator->proto = get_base_iter_proto();
        return data;
    }*/
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
    MpDict* dict = GET_DICT(iterator->data_ptr[0]);
    if (iterator->cur < dict->cap) {
        int i;
        for(i = iterator->cur; i < dict->cap; i++) {
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
MpObj mp_getattr(MpObj obj, char* key) {
    // TODO optimize string find
    MpObj obj_key = string_new(key);
    return obj_get(obj, obj_key);
}


/**
 * get attribute by char array
 * @since 2016-09-02
 */
void mp_setattr(MpObj obj, char* key, MpObj value) {
    MpObj obj_key = string_new(key);
    obj_set(obj, obj_key, value);
}

int mp_hasattr(MpObj obj, char* key) {
    MpObj obj_key = string_const(key);
    DictNode* node = dict_get_node(GET_DICT(obj), obj_key);
    if (node == NULL) {
        return 0;
    }
    return 1;
}
