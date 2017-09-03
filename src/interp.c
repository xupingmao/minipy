/**
  * date : 2014-9-2
  * 2015-6-16: interpreter for tinyvm bytecode.
 **/

#include "include/tm.h"

void tm_loadcode(TmModule* m, char* code);

Object call_function(Object func) {
    Object ret;
    if (IS_FUNC(func)) {
        resolve_method_self(func);
        
        tm_log_call(func);
        
        /* call native */
        if (GET_FUNCTION(func)->native != NULL) {
            return GET_FUNCTION(func)->native();
        } else {
            TmFrame* f = push_frame(func);
            /*
            if (GET_FUNCTION(func)->modifier == 0) {
                return tm_eval(f);
            }*/
            L_recall:
            if (setjmp(f->buf)==0) {
                return tm_eval(f);
            } else {
                f = tm->frame;
                /* handle exception in this frame */
                if (f->cache_jmp != NULL) {
                    f->cache = f->cache_jmp;
                    f->cache_jmp = NULL;
                    goto L_recall;
                /* there is no handler, throw to last frame */
                } else {
                    push_exception(f);
                    tm->frame--;
                    longjmp(tm->frame->buf, 1);
                }
            }
        }
    } else if (IS_DICT(func)) {
        ret = class_new(func);
        Object *_fnc = dict_get_by_str(ret, "__init__");
        if (_fnc != NULL) {
            call_function(*_fnc);
        }
        return ret;
    }
    tm_raise("File %o, line=%d: call_function:invalid object %o", GET_FUNCTION_FILE(tm->frame->fnc), 
        tm->frame->lineno, func);
    return NONE_OBJECT;
}

/**
 * @since 2016-11-20
 */
Object tm_load_module(Object file, Object code, Object name) {
    Object mod = module_new(file, name, code);

    // resolve cache
    tm_loadcode(GET_MODULE(mod), GET_STR(code));

    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = string_new("#main");
    GET_FUNCTION(fnc)->cache = GET_MODULE(mod)->cache;
    call_function(fnc);
    return GET_MODULE(mod)->globals;
}

/**
 * @since 2016-11-27
 */
Object tm_load_module2(char* sz_filename, char* sz_code) {
    Object name = string_new(sz_filename);
    Object file = name;
    Object code = string_new("");
    Object mod = module_new(file, name, code);

    tm_loadcode(GET_MODULE(mod), sz_code);

    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = string_new("#main");
    GET_FUNCTION(fnc)->cache = GET_MODULE(mod)->cache;
    call_function(fnc);
    return GET_MODULE(mod)->globals;
}

/**
 * @since 2016-11-24
 * TODO gc problem, still save string to constants dict?
 */
void tm_loadcode(TmModule* m, char* code) {
    char* s = code;
    char buf[1024];
    int error = 0;
    char* error_msg = NULL;
    
    tm_init_cache(m);
    
    TmCodeCache cache;
    while (*s != 0) {
        /* must begin with opcode */
        if (!isdigit(*s)) {
            error = 1;
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
            for (i = 0;*s != 0 && *s != '\n' && *s != '\r'; s++, i++) {
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
            break;
            error = 1;
        }
        
        // skip \r\n
        while (*s=='\r' || *s=='\n' || *s == ' ' || *s=='\t') {
            s++;
        }
            
        cache.op = op;
        cache.sval = buf; // temp value, just for print
        switch(op) {
            case OP_NUMBER:
                cache.v.obj = tm_number(atof(buf)); break;
            
            /* string value */
            case OP_STRING: 
            case OP_LOAD_GLOBAL:
            case OP_STORE_GLOBAL:
            case OP_FILE: 
            case OP_DEF:           
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
        tm_push_cache(m, cache);
    }
    
    if (error) {
        tm_raise("invalid code");
    }
}

void pop_frame() {
    tm->frame --;
}

#define TM_OP(OP_CODE, OP_FUNC) case OP_CODE: \
    *(top-1) = OP_FUNC(*(top-1), *top);--top;\
    break;
    
#define FRAME_CHECK_GC()  \
f->top = top; \
if (tm->allocated > tm->gc_threshold) {   \
    gc_full();                            \
}

TmFrame* push_frame(Object fnc) {
    /* make extra space for self in method call */
    Object *top = tm->frame->top + 2;
    tm->frame ++ ;
    TmFrame* f = tm->frame;

    /* check oprand stack */
    if (top >= tm->stack + STACK_SIZE) {
        pop_frame();
        tm_raise("tm_eval: stack overflow");
    }
    
    /* check frame stack*/
    if (tm->frame >= tm->frames + FRAMES_COUNT-1) {
        pop_frame();
        tm_raise("tm_eval: frame overflow");
    }

    f->pc = GET_FUNCTION(fnc)->code;
    f->cache = GET_FUNCTION(fnc)->cache;

    f->locals = top;
    f->maxlocals = get_function_max_locals(GET_FUNCTION(fnc));
    f->stack = f->locals + f->maxlocals;
    f->top = f->stack;
    f->fnc = fnc;
 
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
        *top = tm_number(flag); \
    }


/** use cache */
/** 
** evaluate byte code.
** @param f: Frame
** @return evaluated value.
*/
Object tm_eval(TmFrame* f) {
    Object* locals, *top;
    Object cur_fnc, globals, x, k, v, ret;
    TmCodeCache* cache;
    int i;

tailcall:
    locals    = f->locals;
    top       = f->stack;
    cur_fnc    = f->fnc;
    globals    = get_globals(cur_fnc);
    // TODO use code cache to replace unsigned char*
    cache = f->cache;
    const char* func_name_sz = get_func_name_sz(cur_fnc);

    ret = NONE_OBJECT;

    while (1) {
        tm_log_cache(cache);
        #ifdef TM_PRINT_STEPS
            tm->steps++;
        #endif

        switch (cache->op) {

        case OP_NUMBER: {
            TM_PUSH(cache->v.obj);
            break;
        }

        case OP_STRING: {
            TM_PUSH(cache->v.obj);
            break;
        }

        case OP_IMPORT: {
            Object import_func = tm_get_global(globals, "_import");
            arg_start();
            arg_push(globals);
            Object modname, attr;
            
            if (cache->v.ival == 1) {
                modname = TM_POP();
                arg_push(modname); // arg1
            } else {
                attr = TM_POP();
                modname = TM_POP();
                arg_push(modname);
                arg_push(attr);
            }
            call_function(import_func);
            
            break;
        }
        case OP_CONSTANT: {
            TM_PUSH(GET_CONST(cache->v.ival));
            break;
        }
        
        case OP_NONE: {
            TM_PUSH(NONE_OBJECT);
            break;
        }

        case OP_LOAD_LOCAL: {
            TM_PUSH(locals[cache->v.ival]);
            break;
        }

        case OP_STORE_LOCAL:
            locals[cache->v.ival] = TM_POP();
            break;

        case OP_LOAD_GLOBAL: {
            /* tm_printf("load global %o\n", GET_CONST(i)); */
            int idx = dict_get0(GET_DICT(globals), cache->v.obj);
            if (idx == -1) {
                idx = dict_get0(GET_DICT(tm->builtins), cache->v.obj);
                if (idx == -1) {
                    tm_raise("NameError: name %o is not defined", cache->v.obj);
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

                    TM_PUSH(value);
                }
            } else {
                TM_PUSH(GET_DICT(globals)->nodes[idx].val);
                // pc[0] = OP_FAST_LD_GLO;
                // code16(pc+1, idx);
                cache->op = OP_FAST_LD_GLO;
                cache->v.ival = idx;
            }
            break;
        }
        case OP_STORE_GLOBAL: {
            x = TM_POP();
            int idx = dict_set0(GET_DICT(globals), cache->v.obj, x);
            // pc[0] = OP_FAST_ST_GLO;
            // code16(pc+1, idx);
            cache->op = OP_FAST_ST_GLO;
            cache->v.ival = idx;
            break;
        }
        case OP_FAST_LD_GLO: {
            TM_PUSH(GET_DICT(globals)->nodes[cache->v.ival].val);
            break;
        }
        case OP_FAST_ST_GLO: {
            GET_DICT(globals)->nodes[cache->v.ival].val = TM_POP();
            break;
        }
        case OP_LIST: {
            TM_PUSH(list_new(2));
            FRAME_CHECK_GC();
            break;
        }
        case OP_APPEND:
            v = TM_POP();
            x = TM_TOP();
            tm_assert(IS_LIST(x), "tm_eval: OP_APPEND require list");
            list_append(GET_LIST(x), v);
            break;
        case OP_DICT_SET:
            v = TM_POP();
            k = TM_POP();
            x = TM_TOP();
            tm_assert(IS_DICT(x), "tm_eval: OP_DICT_SET require dict");
            obj_set(x, k, v);
            break;
        case OP_DICT: {
            TM_PUSH(dict_new());
            FRAME_CHECK_GC();
            break;
        }
        TM_OP(OP_ADD, obj_add)
        TM_OP(OP_SUB, obj_sub)
        TM_OP(OP_MUL, obj_mul)
        TM_OP(OP_DIV, obj_div)
        TM_OP(OP_MOD, obj_mod)
        TM_OP(OP_GET, obj_get)
        case OP_SLICE: {
            Object second = TM_POP();
            Object first = TM_POP();
            *top = obj_slice(*top, first, second);
            break;
        }
        case OP_EQEQ: { *(top-1) = tm_number(obj_equals(*(top-1), *top)); top--; break; }
        case OP_NOTEQ: { *(top-1) = tm_number(!obj_equals(*(top-1), *top)); top--; break; }
        case OP_LT: {
            *(top-1) = tm_number(obj_cmp(*(top-1), *top)<0);
            top--;
            break;
        }
        case OP_LTEQ: {
            *(top-1) = tm_number(obj_cmp(*(top-1), *top)<=0);
            top--;
            break;
        }
        case OP_GT: {
            *(top-1) = tm_number(obj_cmp(*(top-1), *top)>0);
            top--;
            break;
        }
        case OP_GTEQ: {
            *(top-1) = tm_number(obj_cmp(*(top-1), *top)>=0);
            top--;
            break;
        }
        case OP_IN: {
            *(top-1) = tm_number(obj_in(*(top-1), *top));
            top--;
            break;
        }
        case OP_AND: {
            *(top-1) = tm_number(is_true_obj(*(top-1)) && is_true_obj(*top));
            top--;
            break;
        }
        case OP_OR: {
            *(top-1) = tm_number(is_true_obj(*(top-1)) || is_true_obj(*top));
            top--;
            break;
        }
        case OP_NOT:{
            *top = tm_number(!is_true_obj(*top));
            break;
        }
        case OP_SET:
            k = TM_POP();
            x = TM_POP();
            v = TM_POP();
            obj_set(x, k, v);
            break;
        case OP_POP: {
            top--;
            break;
        }
        case OP_NEG:
            TM_TOP() = obj_neg(TM_TOP());
            break;
        case OP_CALL: {
            int n = cache->v.ival;
            
            f->top = top;
            top -= n;
            arg_set_arguments(top + 1, n);
            Object func = TM_POP();

            TM_PUSH(call_function(func));
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
            Object func = TM_POP();
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
            Object args = TM_POP();
            tm_assert_type(args, TYPE_LIST, "tm_eval: OP_APPLY");
            arg_set_arguments(LIST_NODES(args), LIST_LEN(args));
            Object func = TM_POP();
            x = call_function(func);
            TM_PUSH(x);
            tm->frame = f;
            FRAME_CHECK_GC();
            break;
        }
        case OP_LOAD_PARAMS: {
            int parg = (cache->v.ival >> 8) & 0xff;
            int narg = cache->v.ival & 0xff;
            if (tm->arg_cnt < parg || tm->arg_cnt > parg + narg) {
                tm_raise("ArgError,parg=%d,narg=%d,given=%d", 
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
                TM_PUSH(*next);
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
            TM_PUSH(fnc);
            continue;
        }
        case OP_RETURN: {
            ret = TM_POP();
            goto end;
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
            x = TM_POP();
            tm_assert_type(x, TYPE_LIST, "tm_eval:UNPACK");
            int j;
            for(j = LIST_LEN(x)-1; j >= 0; j--) {
                TM_PUSH(LIST_GET(x, j));
            }
            break;
        }

        case OP_DEL: {
            k = TM_POP();
            x = TM_POP();
            obj_del(x, k);
            break;
        }

        case OP_POP_JUMP_ON_FALSE: {
            if (!is_true_obj(TM_POP())) {
                // pc += i * 3;
                cache += cache->v.ival;
                continue;
            }
            break;
        }

        case OP_JUMP_ON_TRUE: {
            if (is_true_obj(TM_TOP())) {
                cache += cache->v.ival;
                continue;
            }
            break;
        }

        case OP_JUMP_ON_FALSE: {
            if (!is_true_obj(TM_TOP())) {
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

        case OP_LOAD_EX: { top = f->last_top; TM_PUSH(tm->ex); break; }
        case OP_SETJUMP: { 
            f->last_top = top; 
            f->cache_jmp = cache + cache->v.ival;
            break; 
        }
        case OP_CLR_JUMP: { f->jmp = NULL; f->cache_jmp = NULL; break;}
        case OP_LINE: { f->lineno = cache->v.ival; break;}

        case OP_DEBUG: {
            #if 0
            Object fdebug = tm_get_global(globals, "__debug__");
            f->top = top;
            tm_call(0, fdebug, 1, tm_number(tm->frame - tm->frames));        
            break;
            #endif
        }
        
        case OP_FILE: {
            // module name here.
            break;
        }

        default:
            tm_raise("BAD INSTRUCTION, %d\n  globals() = \n%o", cache->op,
                    GET_FUNCTION_GLOBALS(f->fnc));
            goto end;
        }

        cache++;
    }

    end:
    /*
    if (top != f->stack) {
        tm_raise("tm_eval: operand stack overflow");
    }*/
    pop_frame();
    return ret;
}
