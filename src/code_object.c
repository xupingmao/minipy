/*
 * object.h 代码对象
 *
 *  Created on: 2022/06/05 23:55:55
 *  @author: xupingmao
 *  @modified 2022/06/11 19:33:24
 */

#include "include/mp.h"
#include "include/code_cache.h"
#include "log.c"

/**
 * @since 2016-11-24
 */
void mp_resolve_code(MpModule* m, const char* code) {
    const char* s = code;
    char buf[1024];
    int error = 0;
    char* error_msg = NULL;

    memset(buf, 0, sizeof(buf));
    mp_init_cache(m);
    
    MpCodeCache cache;
    cache.op = 0;
    cache.a = 0;
    cache.b = 0;

    MpDict* const_dict = tm->constants;

    while (*s != 0) {
        /* must begin with opcode */
        if (!isdigit(*s)) {
            mp_raise("loadcode: not digit opcode, char=%c, fname=%o, code=%o", *s, m->file, m->code);
            break;
        }
        // read opcode
        int op = 0, i = 0, len = 0;
        /* isdigit -- ctype.h */
        while (isdigit(*s)) {
            op = op * 10 + (*s-'0');
            s++;
        }
        if (*s=='#') {
            s++;
            // load string
            for (i = 0;*s != 0 && *s != '\n' && *s != '\r' && i < sizeof(buf); s++, i++) {
                if (*s=='\\') {
                    s++;
                    switch(*s) {
                        case '\\': buf[i] = '\\'; break;
                        case 'n' : buf[i] = '\n'; break;
                        case 'r' : buf[i] = '\r'; break;
                        case 't' : buf[i] = '\t'; break;
                        case '0' : buf[i] = '\0'; break;
                        default:
                            buf[i] = *(s-1);
                            buf[i+1] = *s;
                            i++;
                    }
                } else {
                    buf[i] = *s;
                }
            }
            buf[i] = '\0';
            len = i;
        } else if (*s == '\n') {
            s++;
            strcpy(buf, "0");
        } else {
            // opcode ended or error
            mp_raise("loadcode: invalid code %d, %c", op, *s);
            break;
        }
        
        // skip \r\n
        while (*s=='\r' || *s=='\n' || *s == ' ' || *s=='\t') {
            s++;
        }

        assert(op >= 0);
        assert(op <= 128);
            
        cache.op = op;
        cache.vtype = 0;
        cache.a = 0;
        cache.b = 0;

        switch(op) {
            case OP_NUMBER: {
                int index = dict_set0(const_dict, mp_number(atof(buf)), tm->_TRUE);
                cache.vtype = CACHE_VTYPE_OBJ;
                if (index >= 0xffff) {
                    mp_raise("mp_resolve_code: constant overflow");
                }
                set_cache_int(&cache, index);
                assert (get_cache_int(&cache) == index);
                break;
            }            
            /* string value */
            case OP_STRING: 
            case OP_LOAD_GLOBAL:
            case OP_STORE_GLOBAL:
            case OP_FILE: 
            case OP_DEF:
            case OP_CLASS: {
                cache.vtype = CACHE_VTYPE_OBJ;
                MpObj str_value = string_const_with_len(buf, len); 
                int index = dict_set0(const_dict, str_value, NONE_OBJECT);
                if (index >= 0xffff) {
                    mp_raise("mp_resolve_code: constant overflow");
                }
                set_cache_int(&cache, index);
                assert (get_cache_int(&cache) == index);
                break;
            }
            /* int value */
            case OP_LOAD_LOCAL:
            case OP_STORE_LOCAL:
            case OP_CALL: 
            case OP_TAILCALL:
            case OP_ROT:
            case OP_JUMP:
            case OP_UP_JUMP:     
            case OP_JUMP_ON_FALSE:
            case OP_JUMP_ON_TRUE: 
            case OP_POP_JUMP_ON_FALSE:
            case OP_LOAD_PARG: 
            case OP_LOAD_NARG:
            case OP_LOAD_PARAMS:
            case OP_LINE:
            case OP_IMPORT:
            case OP_NEXT:
            case OP_SETJUMP: {
                cache.vtype = CACHE_VTYPE_INT;
                int int_value = atoi(buf);
                if (int_value >= 0xffff) {
                    mp_raise("mp_resolve_code: int value overflow");
                }
                set_cache_int(&cache, int_value);
                assert (get_cache_int(&cache) == int_value);
                break;
            }
            default:
                cache.vtype = CACHE_VTYPE_DEFAULT;
                cache.a = 0;
                cache.b = 0;
                break;
        }
        mp_push_cache(m, cache);
        #if MP_DEBUG_CACHE
            log_debug("mp_push_cache:%s,%d", inst_get_name_by_code(cache.op), get_cache_int(&cache));
        #endif
    }
    
    if (error) {
        mp_raise("invalid code");
    }
}


const char* CodeCache_ToString(MpCodeCache* cache) {
	static char buf[1024];
    switch (cache->vtype) {
        case CACHE_VTYPE_DEFAULT: {
            snprintf(buf, 1023, "%-20s\n", 
                inst_get_name_by_code(cache->op));
            break;
        }
        case CACHE_VTYPE_INT: {
            snprintf(buf, 1023, "%-20s o.ival:%d\n", 
                inst_get_name_by_code(cache->op), get_cache_int(cache));
            break;
        }
        case CACHE_VTYPE_OBJ: {
            int index = get_cache_int(cache);
            snprintf(buf, 1023, "%-20s v.obj:%s\n", 
                inst_get_name_by_code(cache->op), mp_get_constant(index));
            break;
        }
        default: {
            int value = get_cache_int(cache);
            snprintf(buf, 1023, "%-20s v.ival:%d\n", 
                inst_get_name_by_code(cache->op), value);
        }
    }
	return buf;
}





#ifdef RECORD_LAST_OP

CodeQueue* CodeQueue_Init(CodeQueue* queue, int cap) {
    assert(cap > 0);
    assert(cap <= 20);
    queue->size = 0;
    queue->start = 0;
    queue->cap = cap;
    return queue;
}

void CodeQueue_Append(CodeQueue* queue, MpCodeCache cache) {
    if (queue->size < queue->cap) {
        queue->size++;
    } else {
        assert(queue->size >= queue->cap);
        queue->start = (++queue->start) % queue->cap;
    }
    int index = queue->start + queue->size - 1;
    int pos = index % queue->cap;
    queue->data[pos] = cache;
}

MpObj CodeQueue_ToString(CodeQueue* queue) {
    int start = queue->start;
    int count = 0;
    MpObj result = string_new("");
    // printf("queue->size=%d\n", queue->size);
    
    while (count < queue->size) {
        MpCodeCache cache = queue->data[start];

        const char* line = CodeCache_ToString(&cache);
        MpObj line_obj = mp_format("%03d: %s", count+1, line);
        string_append_obj(result, line_obj);
        start++;
        start = start % queue->cap;
        count++;
    }

    return result;
}

#endif
