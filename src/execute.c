/**
  * execute minipy bytecode
  * @since 2014-9-2
  * @modified 2020/10/23 00:41:37
  *
  * 2015-6-16: interpreter for tinyvm bytecode.
 **/

#include "include/mp.h"

void mp_resolve_code(MpModule* m, char* code);

Object call_function(Object func) {
    Object ret;
    if (IS_FUNC(func)) {
        resolve_method_self(func);
        mp_log_call(func);
        
        /* call native function */
        if (GET_FUNCTION(func)->native != NULL) {
            return GET_FUNCTION(func)->native();
        } else {
            MpFrame* f = push_frame(func);

            L_recall:
            if (setjmp(f->buf)==0) {
                return mp_eval(f);
            } else {
                f = tm->frame;
                /* handle exception in this frame */
                if (f->cache_jmp != NULL) {
                    f->cache = f->cache_jmp;
                    f->cache_jmp = NULL;
                    goto L_recall;
                } else {
                    /* there is no handler, throw to prev frame */
                    push_exception(f);
                    pop_frame();
                    longjmp(tm->frame->buf, 1);
                }
            }
        }
    } else if (IS_CLASS(func)) {
        ret = class_instance(func);
        Object *_fnc = dict_get_by_str(ret, "__init__");
        if (_fnc != NULL) {
            call_function(*_fnc);
        }
        return ret;
    }
    mp_raise("File %o, line=%d: call_function:invalid object %o", GET_FUNCTION_FILE(tm->frame->fnc), 
        tm->frame->lineno, func);
    return NONE_OBJECT;
}

/**
 * @since 2016-11-24
 * TODO gc problem, still save string to constants dict?
 */
void mp_resolve_code(MpModule* m, char* code) {
    char* s = code;
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
            error = 1;
        }
        
        // skip \r\n
        while (*s=='\r' || *s=='\n' || *s == ' ' || *s=='\t') {
            s++;
        }
            
        cache.op = op;
        cache.v.ival = 0;
        cache.sval = buf; // temp value, just for print
        switch(op) {
            case OP_NUMBER:
                cache.v.obj = number_obj(atof(buf)); break;
            
            /* string value */
            case OP_STRING: 
            case OP_LOAD_GLOBAL:
            case OP_STORE_GLOBAL:
            case OP_FILE: 
            case OP_DEF:
            case OP_CLASS:
                cache.v.obj = string_const2(buf, len); break;
            
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
                cache.v.ival = atoi(buf); break;
            default:
                cache.v.ival = 0; break;
        }
        mp_push_cache(m, cache);
    }
    
    if (error) {
        mp_raise("invalid code");
    }
}

void pop_frame() {
    if (tm->frame < tm->frames) {
        printf("pop_frame: invalid call\n");
        exit(1);
    }
    tm->frame --;
}

#define MP_OP(OP_CODE, OP_FUNC) case OP_CODE: \
    *(top-1) = OP_FUNC(*(top-1), *top);--top;\
    break;
    
#define FRAME_CHECK_GC()  \
f->top = top; \
if (tm->allocated > tm->gc_threshold) {   \
    gc_full();                            \
}

MpFrame* push_frame(Object fnc) {
    /* make extra space for self in method call */
    Object *top = tm->frame->top + 2;
    tm->frame ++ ;
    MpFrame* f = tm->frame;

    /* check oprand stack */
    if (top >= tm->stack + STACK_SIZE) {
        pop_frame();
        mp_raise("mp_eval: stack overflow");
    }
    
    /* check frame stack*/
    if (tm->frame >= tm->frames + FRAMES_COUNT-1) {
        pop_frame();
        mp_raise("mp_eval: frame overflow");
    }

    f->pc    = GET_FUNCTION(fnc)->code;
    f->cache = GET_FUNCTION(fnc)->cache;

    f->maxlocals = get_function_max_locals(GET_FUNCTION(fnc));

    f->locals = top;
    /* stack starts after locals */
    f->stack  = f->locals + f->maxlocals;
    f->top    = f->stack;
    f->fnc    = fnc;
 
    // clear local variables
    int i;
    for(i = 0; i < f->maxlocals; i++) {
        f->locals[i] = NONE_OBJECT;
    }
    *(f->top) = NONE_OBJECT;
    f->jmp = NULL;
    f->cache_jmp = NULL;
    return f;
}

#define PREDICT_JMP(flag) \
    if (!flag && pc[3] == OP_POP_JUMP_ON_FALSE) { \
        top--;\
        pc += 3; \
        i = (pc[1] << 8) | pc[2]; \
        pc += i * 3; \
        continue;\
    } else { \
        *top = number_obj(flag); \
    }


/** use cache */
/** 
** evaluate byte code.
** @param f: Frame
** @return evaluated value.
*/
Object mp_eval(MpFrame* f) {
    Object* locals, *top;
    Object cur_fnc, globals, x, k, v, ret;
    MpCodeCache* cache;
    int i;

tailcall:
    locals  = f->locals;
    top     = f->stack;
    cur_fnc = f->fnc;
    globals = GET_GLOBALS(cur_fnc);
    cache   = f->cache;

    const char* func_name_sz = get_func_name_sz(cur_fnc);

    ret = NONE_OBJECT;

    while (1) {
        mp_log_cache(cache);
        #ifdef MP_PRINT_STEPS
            tm->steps++;
        #endif

        switch (cache->op) {

        case OP_NUMBER: {
            MP_PUSH(cache->v.obj);
            break;
        }

        case OP_STRING: {
            MP_PUSH(cache->v.obj);
            break;
        }

        case OP_IMPORT: {
            // _import(des_globals, fname, tar);
            Object import_func = mp_get_global(globals, "_import");
            arg_start();
            arg_push(globals);
            Object modname, attr;

            if (cache->v.ival == 1) {
                modname = MP_POP();
                arg_push(modname); // arg1
            } else {
                attr = MP_POP();
                modname = MP_POP();
                arg_push(modname);
                arg_push(attr);
            }
            call_function(import_func);
            break;
        }
        case OP_CONSTANT: {
            MP_PUSH(GET_CONST(cache->v.ival));
            break;
        }
        
        case OP_NONE: {
            MP_PUSH(NONE_OBJECT);
            break;
        }

        case OP_LOAD_LOCAL: {
            MP_PUSH(locals[cache->v.ival]);
            break;
        }

        case OP_STORE_LOCAL:
            locals[cache->v.ival] = MP_POP();
            break;

        case OP_LOAD_GLOBAL: {
            /* mp_printf("load global %o\n", GET_CONST(i)); */
            int idx = dict_get0(GET_DICT(globals), cache->v.obj);
            if (idx == -1) {
                idx = dict_get0(GET_DICT(tm->builtins), cache->v.obj);
                if (idx == -1) {
                    mp_raise("NameError: name %o is not defined", cache->v.obj);
                } else {
                    Object value = GET_DICT(tm->builtins)->nodes[idx].val;
                    // OPTIMIZE
                    // set the builtin to `globals()`
                    obj_set(globals, cache->v.obj, value);
                    idx = dict_get0(GET_DICT(globals), cache->v.obj);
                    // pc[0] = OP_FAST_LD_GLO;
                    // code16(pc+1, idx);
                    // OPTIMIZE END
                    cache->op = OP_FAST_LD_GLO;
                    cache->v.ival = idx;

                    MP_PUSH(value);
                }
            } else {
                MP_PUSH(GET_DICT(globals)->nodes[idx].val);
                // pc[0] = OP_FAST_LD_GLO;
                // code16(pc+1, idx);
                cache->op = OP_FAST_LD_GLO;
                cache->v.ival = idx;
            }
            break;
        }
        case OP_STORE_GLOBAL: {
            x = MP_POP();
            int idx = dict_set0(GET_DICT(globals), cache->v.obj, x);
            // pc[0] = OP_FAST_ST_GLO;
            // code16(pc+1, idx);
            cache->op = OP_FAST_ST_GLO;
            cache->v.ival = idx;
            break;
        }
        case OP_FAST_LD_GLO: {
            MP_PUSH(GET_DICT(globals)->nodes[cache->v.ival].val);
            break;
        }
        case OP_FAST_ST_GLO: {
            GET_DICT(globals)->nodes[cache->v.ival].val = MP_POP();
            break;
        }
        case OP_LIST: {
            MP_PUSH(list_new(2));
            FRAME_CHECK_GC();
            break;
        }
        case OP_APPEND:
            v = MP_POP();
            x = MP_TOP();
            mp_assert(IS_LIST(x), "mp_eval: OP_APPEND require list");
            list_append(GET_LIST(x), v);
            break;
        case OP_DICT_SET:
            v = MP_POP();
            k = MP_POP();
            x = MP_TOP();
            mp_assert(IS_DICT(x), "mp_eval: OP_DICT_SET require dict");
            obj_set(x, k, v);
            break;
        case OP_DICT: {
            MP_PUSH(dict_new());
            FRAME_CHECK_GC();
            break;
        }
        MP_OP(OP_ADD, obj_add)
        MP_OP(OP_SUB, obj_sub)
        MP_OP(OP_MUL, obj_mul)
        MP_OP(OP_DIV, obj_div)
        MP_OP(OP_MOD, obj_mod)
        MP_OP(OP_GET, obj_get)
        case OP_SLICE: {
            Object second = MP_POP();
            Object first  = MP_POP();
            *top = obj_slice(*top, first, second);
            break;
        }
        case OP_EQEQ: { *(top-1) = number_obj(obj_equals(*(top-1), *top)); top--; break; }
        case OP_NOTEQ: { *(top-1) = number_obj(!obj_equals(*(top-1), *top)); top--; break; }
        case OP_LT: {
            *(top-1) = number_obj(mp_cmp(*(top-1), *top)<0);
            top--;
            break;
        }
        case OP_LTEQ: {
            *(top-1) = number_obj(mp_cmp(*(top-1), *top)<=0);
            top--;
            break;
        }
        case OP_GT: {
            *(top-1) = number_obj(mp_cmp(*(top-1), *top)>0);
            top--;
            break;
        }
        case OP_GTEQ: {
            *(top-1) = number_obj(mp_cmp(*(top-1), *top)>=0);
            top--;
            break;
        }
        case OP_IN: {
            *(top-1) = number_obj(mp_in(*(top-1), *top));
            top--;
            break;
        }
        case OP_AND: {
            *(top-1) = number_obj(is_true_obj(*(top-1)) && is_true_obj(*top));
            top--;
            break;
        }
        case OP_OR: {
            *(top-1) = number_obj(is_true_obj(*(top-1)) || is_true_obj(*top));
            top--;
            break;
        }
        case OP_NOT:{
            *top = number_obj(!is_true_obj(*top));
            break;
        }
        case OP_SET:
            k = MP_POP();
            x = MP_POP();
            v = MP_POP();
            obj_set(x, k, v);
            break;
        case OP_POP: {
            top--;
            break;
        }
        case OP_NEG:
            MP_TOP() = obj_neg(MP_TOP());
            break;
        case OP_CALL: {
            int n = cache->v.ival;
            
            f->top = top;
            top -= n;
            /* TODO top+1 can be optimized as locals */
            arg_set_arguments(top + 1, n);
            Object func = MP_POP();

            MP_PUSH(call_function(func));
            tm->frame = f;
            FRAME_CHECK_GC();
            break;
        }
        case OP_TAILCALL: {
            int n = cache->v.ival;
            f->top = top;
            top -= n;
            Object* first_arg = top+1;
            arg_set_arguments(top+1, n);
            Object func = MP_POP();
            if (GET_FUNCTION(func)->native == NULL) {
                /** tail call python function **/
                arg_start();
                int i = 0;
                for (i = 0; i < n; i++) {
                    arg_push(first_arg[i]);
                }
                resolve_method_self(func);
                f->fnc = func;
                f->pc = GET_FUNCTION(func)->code;
                f->cache = GET_FUNCTION(func)->cache;
                f->maxlocals = get_function_max_locals(GET_FUNCTION(func));
                f->stack = f->locals + f->maxlocals;
                f->top = f->stack;
                // clear locals
                for (i = 0; i < f->maxlocals; i++) {
                    f->locals[i] = NONE_OBJECT;
                }
                goto tailcall;
            } else {
                return call_function(func);
            }
            break;
        }
        case OP_APPLY: {
            f->top = top;
            Object args = MP_POP();
            mp_assert_type(args, TYPE_LIST, "mp_eval: OP_APPLY");
            arg_set_arguments(LIST_NODES(args), LIST_LEN(args));
            Object func = MP_POP();
            x = call_function(func);
            MP_PUSH(x);
            tm->frame = f;
            FRAME_CHECK_GC();
            break;
        }
        case OP_LOAD_PARAMS: {
            int parg = (cache->v.ival >> 8) & 0xff;
            int narg = cache->v.ival & 0xff;
            if (tm->arg_cnt < parg || tm->arg_cnt > parg + narg) {
                mp_raise("ArgError,parg=%d,narg=%d,given=%d", 
                    parg, narg, tm->arg_cnt);
            }
            int i;
            for(i = 0; i < tm->arg_cnt; i++){
                locals[i] = tm->arguments[i];
            }
            break;
        }
        case OP_LOAD_PARG: {
            int parg = cache->v.ival;
            for (i = 0; i < parg; i++) {
                locals[i] = arg_take_obj(func_name_sz);
            }
            break;
        }
        case OP_LOAD_NARG: {
            int arg_index = cache->v.ival;
            Object list = list_new(tm->arg_cnt);
            while (arg_remains() > 0) {
                obj_append(list, arg_take_obj(func_name_sz));
            }
            locals[arg_index] = list;
            break;
        }
        case OP_ITER: {
            *top = iter_new(*top);
            break;
        }
        case OP_NEXT: {
            Object *next = next_ptr(*top);
            if (next != NULL) {
                MP_PUSH(*next);
                break;
            } else {
                // pc += i * 3;
                cache += cache->v.ival;
                continue;
            }
            break;
        }
        case OP_DEF: {
            Object mod = GET_FUNCTION(cur_fnc)->mod;
            Object fnc = func_new(mod, NONE_OBJECT, NULL);
            GET_FUNCTION_NAME(fnc) = cache->v.obj;
            cache = func_resolve_cache(GET_FUNCTION(fnc), cache);
            MP_PUSH(fnc);
            continue;
        }
        case OP_RETURN: {
            ret = MP_POP();
            goto end;
        }
        case OP_CLASS: {
            Object class_name = cache->v.obj;
            Object clazz = class_new(class_name);
            dict_set0(GET_DICT(globals), class_name, clazz);
            break;
        }
        case OP_ROT: {
            Object* left = top - cache->v.ival + 1;
            Object* right = top;
            for (; left < right; left++, right--) {
                Object temp = *left;
                *left = *right;
                *right = temp;
            }
            break;
        }
        case OP_UNPACK: {
            x = MP_POP();
            mp_assert_type(x, TYPE_LIST, "mp_eval:UNPACK");
            int j;
            for(j = LIST_LEN(x)-1; j >= 0; j--) {
                MP_PUSH(LIST_GET(x, j));
            }
            break;
        }

        case OP_DEL: {
            k = MP_POP();
            x = MP_POP();
            obj_del(x, k);
            break;
        }

        case OP_POP_JUMP_ON_FALSE: {
            if (!is_true_obj(MP_POP())) {
                // pc += i * 3;
                cache += cache->v.ival;
                continue;
            }
            break;
        }

        case OP_JUMP_ON_TRUE: {
            if (is_true_obj(MP_TOP())) {
                cache += cache->v.ival;
                continue;
            }
            break;
        }

        case OP_JUMP_ON_FALSE: {
            if (!is_true_obj(MP_TOP())) {
                cache += cache->v.ival;
                continue;
            }
            break;
        }

        case OP_UP_JUMP:
            cache -= cache->v.ival;
            continue;

        case OP_JUMP:
            cache += cache->v.ival;
            continue;

        case OP_EOP:
        case OP_EOF: {
           ret = NONE_OBJECT;
           goto end;
        }

        case OP_LOAD_EX: { top = f->last_top; MP_PUSH(tm->ex); break; }
        case OP_SETJUMP: { 
            f->last_top = top; 
            f->cache_jmp = cache + cache->v.ival;
            break; 
        }
        case OP_CLR_JUMP: { f->jmp = NULL; f->cache_jmp = NULL; break;}
        case OP_LINE: { f->lineno = cache->v.ival; break;}

        case OP_DEBUG: {
            #if 0
            Object fdebug = mp_get_global(globals, "__debug__");
            f->top = top;
            mp_call(0, fdebug, 1, number_obj(tm->frame - tm->frames));        
            break;
            #endif
        }
        
        case OP_FILE: {
            // module name here.
            break;
        }

        default:
            mp_raise("BAD INSTRUCTION, %d\n  globals() = \n%o", cache->op,
                    GET_FUNCTION_GLOBALS(f->fnc));
            goto end;
        }

        cache++;
    }

    end:
    /*
    if (top != f->stack) {
        mp_raise("mp_eval: operand stack overflow");
    }*/
    pop_frame();
    return ret;
}
