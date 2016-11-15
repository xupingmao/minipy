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
    unsigned char* pc = f->pc;
    const char* func_name_sz = get_func_name_sz(cur_fnc);

    Object x, k, v;
    Object ret = NONE_OBJECT;
    int i;

    #if INTERP_DB
        printf("File \"%s\": enter function %s\n",get_func_file_sz(cur_fnc), get_func_name_sz(cur_fnc));
    #endif
    while (1) {
        i = (pc[1] << 8) | pc[2];
        
        #if INTERP_DB
            printf("%30s%2d: %d frame = %d, top = %d\n","", pc[0], i, tm->cur, (int) (top - f->stack));
        #endif    
        switch (pc[0]) {

        case OP_NUMBER: {
            double d = atof((char*)pc + 3);
            pc += i;
            v = tm_number(d);
            /* obj_append(tm->constants,v);*/
            dict_set(tm->constants, v, NONE_OBJECT);
            break;
        }

        case OP_STRING: {
            v = string_alloc((char*)pc + 3, i);
            pc += i;
            /* obj_append(tm->constants,v); */
            dict_set(tm->constants, v, NONE_OBJECT);
            break;
        }

        case OP_IMPORT: {
            Object import_func = tm_get_global(globals, sz_to_string("_import"));
            arg_start();
            arg_push(globals);
            if (i == 1) {
                arg_push(TM_POP());
            } else {
                Object b = TM_POP();
                Object a = TM_POP();
                arg_push(a);
                arg_push(b);
            }
            call_function(import_func);
            break;
        }
        case OP_CONSTANT: {
            TM_PUSH(GET_CONST(i));
            break;
        }
        
        case OP_NONE: {
            TM_PUSH(NONE_OBJECT);
            break;
        }

        case OP_LOAD_LOCAL: {
            TM_PUSH(locals[i]);
            break;
        }

        case OP_STORE_LOCAL:
            locals[i] = TM_POP();
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
            f->top = top;
            top -= i;
            arg_set_arguments(top + 1, i);
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
