/**
  * date : 2014-9-2
  * 2015-6-16: interpreter for tinyvm bytecode.
 **/

#include "include/tm.h"

#define INTERP_DB 0

Object call_function(Object func) {
    Object ret;
    if (IS_FUNC(func)) {
        resolve_method_self(func);
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
                if (f->jmp != NULL) {
                    f->pc = f->jmp;
                    f->jmp = NULL;
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
    Object fnc = func_new(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = string_new("#main");
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
        int op = 0;
        /* isdigit -- ctype.h */
        while (isdigit(*s)) {
            op = op * 10 + (*s-'0');
            s++;
        }
        if (*s=='#') {
            s++;
        } else {
            // opcode ended or error
            break;
            error = 1;
        }
        
        int i = 0;
        // load string
        for (i = 0;*s != 0 && *s != '\n' && *s != '\r'; s++, i++) {
            if (*s=='\\') {
                s++;
                switch(*s) {
                    case '\\': buf[i] = '\\'; break;
                    case 'n' : buf[i] = '\n'; break;
                    case 'r' : buf[i] = '\r'; break;
                    case 't' : buf[i] = '\t'; break;
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
                cache.v.obj = string_const(buf); break;
            
            /* int value */
            case OP_LOAD_LOCAL:
            case OP_STORE_LOCAL:
            case OP_CALL: 
            case OP_ROT:
            case OP_JUMP:
            case OP_UP_JUMP:     
            case OP_JUMP_ON_FALSE:
            case OP_JUMP_ON_TRUE: 
            case OP_LOAD_PARG: 
            case OP_LOAD_NARG:
            case OP_LINE:
                cache.v.ival = atoi(buf); break;
        }
        tm_push_cache(m, cache);
    }
    
    if (error) {
        tm_raise("invalid code");
    }
}

/**
 * @since 2016-11-15
 */
void tm_import(Object globals, Object modname, Object attr) {
    Object mod;
    if (obj_in(modname, tm->modules)) {
        mod = obj_get(tm->modules, modname);
    } else {
        // compile and import
        Object ext = string_const(".py");
        Object filename = obj_add(modname, ext);
        Object code = call_mod_func("encode", "compilefile");
        mod = tm_load_module(filename, modname, code);
    }
    Object star = string_const("*");
    if (obj_equals(attr, star)) {
        // set all attribute
        int i;
        for (i = 0; i < DICT_LEN(mod); i++) {
            DictNode node = DICT_NODES(mod)[i];
            // filter attr starts with _
            Object key = node.key;
            if (IS_STR(key) && GET_STR(key)[0] != '_') {
                obj_set(globals, node.key, node.val);
            }
        }
    } else if (IS_NONE(attr)) {
        obj_set(globals, modname, mod);
    } else {
        // get one attribute;
        Object v = obj_get(mod, attr);
        obj_set(globals, attr, v);
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

    f->locals = top;
    f->maxlocals = get_function_max_locals(GET_FUNCTION(fnc));
    f->stack = f->locals + f->maxlocals;
    f->top = f->stack;
    f->fnc = fnc;
 
    // clear local variables
    int i;for(i = 0; i < f->maxlocals; i++) {
        f->locals[i] = NONE_OBJECT;
    }
    *(f->top) = NONE_OBJECT;
    f->jmp = NULL;
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
    Object* locals    = f->locals;
    Object* top       = f->stack;
    Object cur_fnc    = f->fnc;
    Object globals    = get_globals(cur_fnc);
    // TODO use code cache to replace unsigned char*
    TmCodeCache* cache = f->cache;
    const char* func_name_sz = get_func_name_sz(cur_fnc);

    Object x, k, v;
    Object ret = NONE_OBJECT;
    int i;

    #if INTERP_DB
        printf("File \"%s\": enter function %s\n",get_func_file_sz(cur_fnc), get_func_name_sz(cur_fnc));
    #endif
    while (1) {
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
            Object modname, attr;
            
            if (i == 1) {
                modname = TM_POP();
                attr = NONE_OBJECT;
                // arg_push(TM_POP()); // arg1
            } else {
                attr = TM_POP();
                modname = TM_POP();
                // arg_push(a);
                // arg_push(b);
            }
            tm_import(globals, modname, attr);
            // all_function(import_func);
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
            int idx = dict_get_attr(GET_DICT(globals), i);
            if (idx == -1) {
                idx = dict_get_attr(GET_DICT(tm->builtins), i);
                if (idx == -1) {
                    tm_raise("NameError: name %o is not defined", GET_CONST(i));
                } else {
                    Object value = GET_DICT(tm->builtins)->nodes[idx].val;
                    // OPTIMIZE
                    // set the builtin to `globals()`
                    obj_set(globals, GET_CONST(i), value);
                    idx = dict_get_attr(GET_DICT(globals), i);
                    pc[0] = OP_FAST_LD_GLO;
                    code16(pc+1, idx);
                    // OPTIMIZE END
                    TM_PUSH(value);
                }
            } else {
                TM_PUSH(GET_DICT(globals)->nodes[idx].val);
                pc[0] = OP_FAST_LD_GLO;
                code16(pc+1, idx);
            }
            break;
        }
        case OP_STORE_GLOBAL: {
            x = TM_POP();
            int idx = dict_set_attr(GET_DICT(globals), i, x);
            pc[0] = OP_FAST_ST_GLO;
            code16(pc+1, idx);
            break;
        }
        case OP_FAST_LD_GLO: {
            TM_PUSH(GET_DICT(globals)->nodes[i].val);
            break;
        }
        case OP_FAST_ST_GLO: {
            GET_DICT(globals)->nodes[i].val = TM_POP();
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
            tm_assert_type(x, TYPE_LIST, "tm_eval: OP_APPEND");
            list_append(GET_LIST(x), v);
            break;
        case OP_DICT_SET:
            v = TM_POP();
            k = TM_POP();
            x = TM_TOP();
            tm_assert_type(x, TYPE_DICT, "tm_eval: DICT_SET");
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
            #if INTERP_DB
                tm_printf("Self %o, Key %o, Val %o\n", x, k, v);
            #endif
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
            #if INTERP_DB
                printf("call %s\n", get_func_name_sz(func));
            #endif

            x = call_function(func);
            TM_PUSH(x);
            // TM_PUSH(call_function(func));
            tm->frame = f;
            FRAME_CHECK_GC();
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
            int parg = pc[1];
            int varg = pc[2];
            if (tm->arg_cnt < parg || tm->arg_cnt > parg + varg) {
                tm_raise("ArgError,parg=%d,varg=%d,given=%d", 
                    parg, varg, tm->arg_cnt);
            }
            for(i = 0; i < tm->arg_cnt; i++){
                locals[i] = tm->arguments[i];
            }
            break;
        }
        case OP_LOAD_PARG: {
            int parg = i;
            for (i = 0; i < parg; i++) {
                locals[i] = arg_take_obj(func_name_sz);
            }
            break;
        }
        case OP_LOAD_NARG: {
            int arg_index = i;
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
                pc += i * 3;
                continue;
            }
            break;
        }
        case OP_DEF: {
            Object mod = GET_FUNCTION(cur_fnc)->mod;
            Object fnc = func_new(mod, NONE_OBJECT, NULL);
            pc = func_resolve(GET_FUNCTION(fnc), pc);
            GET_FUNCTION_NAME(fnc) = GET_CONST(i);
            TM_PUSH(fnc);
            continue;
        }
        case OP_RETURN: {
            ret = TM_POP();
            goto end;
        }
        case OP_ROT: {
            int half = i / 2;
            int j;
            for (j = 0; j < half; j++) {
                Object temp = *(top - j);
                *(top-j) = *(top - i + j + 1);
                *(top-i+j+1) = temp;
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
                pc += i * 3;
                continue;
            }
            break;
        }

        case OP_JUMP_ON_TRUE: {
            if (is_true_obj(TM_TOP())) {
                pc += i * 3;
                continue;
            }
            break;
        }

        case OP_JUMP_ON_FALSE: {
            if (!is_true_obj(TM_TOP())) {
                pc += i * 3;
                continue;
            }
            break;
        }

        case OP_UP_JUMP:
            pc -= i * 3;
            continue;

        case OP_JUMP:
            pc += i * 3;
            continue;

        case OP_EOP:
        case OP_EOF: {
           ret = NONE_OBJECT;
           goto end;
        }

        case OP_LOAD_EX: { top = f->last_top; TM_PUSH(tm->ex); break; }
        case OP_SETJUMP: { f->last_top = top; f->jmp = pc + i * 3; break; }
        case OP_CLR_JUMP: { f->jmp = NULL; break;}
        case OP_LINE: { f->lineno = i; break;}

        case OP_DEBUG: {
            #if 0
            Object fdebug = tm_get_global(globals, sz_to_string("__debug__"));
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
            tm_raise("BAD INSTRUCTION, %d\n  globals() = \n%o", pc[0],
                    GET_FUNCTION_GLOBALS(f->fnc));
            goto end;
        }

        pc += 3;
    }

    end:
    /*
    if (top != f->stack) {
        tm_raise("tm_eval: operand stack overflow");
    }*/
    pop_frame();
    return ret;
}
