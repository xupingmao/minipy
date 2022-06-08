/*
 * object.h 代码对象
 *
 *  Created on: 2022/06/05 23:55:55
 *  @author: xupingmao
 *  @modified 2022/06/08 23:18:43
 */

#include "include/mp.h"

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
    cache.v.ival = 0;
    cache.sval = NULL;

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
            
        cache.op = op;
        cache.vtype = 0;
        cache.v.ival = 0;
        cache.index = -1;
        cache.sval = buf; // temp value, just for print
        switch(op) {
            case OP_NUMBER:
                cache.v.obj = number_obj(atof(buf)); 
                cache.vtype = CACHE_VTYPE_OBJ;
                break;
            
            /* string value */
            case OP_STRING: 
            case OP_LOAD_GLOBAL:
            case OP_STORE_GLOBAL:
            case OP_FILE: 
            case OP_DEF:
            case OP_CLASS:
                cache.v.obj = string_const2(buf, len); 
                cache.vtype = CACHE_VTYPE_OBJ;
                break;
            
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
            case OP_SETJUMP:
                cache.v.ival = atoi(buf); 
                cache.vtype = CACHE_VTYPE_INT;
                break;
            default:
                cache.v.ival = 0; 
                break;
        }
        mp_push_cache(m, cache);
    }
    
    if (error) {
        mp_raise("invalid code");
    }
}


const char* CodeCache_ToString(MpCodeCache* cache) {
	static char buf[1024];
    switch (cache->vtype) {
        case CACHE_VTYPE_INT: {
            snprintf(buf, 1023, "%-20s o.ival:%d\n", 
                inst_get_name_by_code(cache->op), cache->v.ival);
            break;
        }
        case CACHE_VTYPE_OBJ: {
            snprintf(buf, 1023, "%-20s v.obj:%s\n", 
                inst_get_name_by_code(cache->op), obj_to_cstr(cache->v.obj));
            break;
        }
        case CACHE_VTYPE_STR: {
            snprintf(buf, 1023, "%-20s v.sval:%s\n", 
                inst_get_name_by_code(cache->op), cache->sval);
            break;
        }
        default: {
            snprintf(buf, 1023, "%-20s v.ival:%d\n", 
                inst_get_name_by_code(cache->op), cache->v.ival);
        }
    }
	return buf;
}