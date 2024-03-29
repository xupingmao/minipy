/**
  * execute minipy bytecode
  * @since 2014-9-2
  * @modified 2022/06/11 21:44:05
  *
  * 2015-6-16: interpreter for tinyvm bytecode.
 **/

#include "include/mp.h"
#include "execute_profile.c"

void pop_frame() {
    if (tm->frame < tm->frames) {
        printf("pop_frame: invalid call\n");
        exit(1);
    }
    tm->frame --;
}

#define MP_PUSH(x) *(++top) = (x); \
    assert(top->type >= TYPE_MIN && top->type <= TYPE_MAX); \
    if(top > tm->stack_end) \
        mp_raise("mp_eval: stack overflow");


#define MP_PUSH_TEST(x) ++top;   \
    top->type = (x).type;   \
    top->value = (x).value; \
    if(top > tm->stack_end)        \
        mp_raise("mp_eval: stack overflow");

#define MP_POP() *(top--)
#define MP_TOP() (*top)

#define MP_OP(OP_CODE, OP_FUNC) case OP_CODE: {\
    PROFILE_START(cache); \
    *(top-1) = OP_FUNC(*(top-1), *top);--top;\
    PROFILE_END(cache); \
    break;\
    }
    

#define FRAME_CHECK_GC()  \
f->top = top; \
if (tm->allocated > tm->gc_threshold) {   \
    gc_full();                            \
}

MpFrame* push_frame(MpObj fnc) {
    // make extra space for self in method call
    // top包含当前frame的stack-value
    // top+1 需要为self预留空间
    MpObj *top = tm->frame->top + 2;
    tm->frame ++ ;
    MpFrame* f = tm->frame;

    /* check oprand stack */
    if (top >= tm->stack + STACK_SIZE) {
        pop_frame();
        mp_raise("mp_eval: stack overflow (%d)", STACK_SIZE);
    }
    
    /* check frame stack*/
    if (tm->frame >= tm->frames + FRAMES_COUNT-1) {
        pop_frame();
        mp_raise("mp_eval: frame overflow (%d)", FRAMES_COUNT-1);
    }

    f->pc    = GET_FUNCTION(fnc)->code;
    f->cache = GET_FUNCTION(fnc)->cache;

    assert(GET_FUNCTION(fnc)->resolved == 1);

    f->maxlocals = func_get_max_locals(GET_FUNCTION(fnc));

    f->locals = top;
    /* stack starts after locals */
    f->stack  = f->locals + f->maxlocals;
    f->top    = f->stack;
    f->fnc    = fnc;
 
    // clear local variables
    int i = 0;
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
MpObj mp_eval(MpFrame* f) {
    MpObj* locals, *top;
    MpObj cur_fnc, globals, x, k, v, ret;
    MpCodeCache* cache;
    int i = 0;

    if (MP_TYPE(f->fnc) != TYPE_FUNCTION) {
        mp_raise("mp_eval: expect func but see type:%d", MP_TYPE(f->fnc));
    }

tailcall:
    locals  = f->locals;
    top     = f->stack;
    cur_fnc = f->fnc;
    globals = obj_get_globals(cur_fnc);
    MpDict *globals_dict = GET_DICT(globals);
    cache   = f->cache;

    const char* func_name_cstr = func_get_name_cstr(cur_fnc);

    ret = NONE_OBJECT;

    while (1) {
        // 用于统计性能分析函数本身的性能
        PROFILE_NULL_START();
        PROFILE_NULL_END();

        #ifdef MP_PRINT_STEPS
            tm->steps++;
        #endif

        #ifdef RECORD_LAST_OP
            CodeQueue_Append(&tm->last_op_queue, *cache);
        #endif

retry_op:
        switch (cache->op) {

        case OP_NUMBER: {
            PROFILE_START(cache);
            MP_PUSH(cache->v.obj);
            PROFILE_END(cache);
            break;
        }

        case OP_STRING: {
            PROFILE_START(cache);
            MP_PUSH(cache->v.obj);
            PROFILE_END(cache);
            break;
        }

        case OP_IMPORT: {
            // _import(des_globals, fname, tar);
            if (cache->v.ival == 1) {
                // import name
                MpObj modname = MP_POP();
                obj_import(globals, modname);
            } else {
                // from name import attr
                MpObj attr = MP_POP();
                MpObj modname = MP_POP();
                obj_import_attr(globals, modname, attr);
            }
            break;
        }
        case OP_CONSTANT: {
            PROFILE_START(cache);
            MP_PUSH(GET_CONST(cache->v.ival));
            PROFILE_END(cache);
            break;
        }
        
        case OP_NONE: {
            PROFILE_START(cache);
            MP_PUSH(NONE_OBJECT);
            PROFILE_END(cache);
            break;
        }

        case OP_LOAD_LOCAL: {
            PROFILE_START(cache);
            MP_PUSH(locals[cache->v.ival]);
            PROFILE_END(cache);
            break;
        }

        case OP_STORE_LOCAL:
            PROFILE_START(cache);
            locals[cache->v.ival] = MP_POP();
            PROFILE_END(cache);
            break;

        case OP_LOAD_GLOBAL: {
            PROFILE_START(cache);
            /* mp_printf("load global %o\n", GET_CONST(i)); */
            int idx = dict_get0(globals_dict, cache->v.obj);
            if (idx == -1) {
                MpDict *builtins_dict = GET_DICT(tm->builtins);
                idx = dict_get0(builtins_dict, cache->v.obj);
                if (idx == -1) {
                    mp_raise("NameError: name %o is not defined", cache->v.obj);
                } else {
                    MpObj value = builtins_dict->nodes[idx].val;
                    MP_PUSH(value);
                }
            } else {
                MP_PUSH(globals_dict->nodes[idx].val);

                #ifdef FAST_LOAD_GLOBAL
                    cache->op = OP_LOAD_GLOBAL_FAST;
                    cache->index = idx;
                #endif
            }

            PROFILE_END(cache);
            break;
        }
        case OP_STORE_GLOBAL: {
            PROFILE_START(cache);
            MpObj value = MP_POP();
            int idx = dict_set0(globals_dict, cache->v.obj, value);
            // cache->op = OP_STORE_GLOBAL_FAST;
            // cache->index = idx;
            PROFILE_END(cache);
            break;
        }
        case OP_LOAD_GLOBAL_FAST: {
            PROFILE_START(cache);
            // 需要对比一下key是否匹配,处理命中失败的情况
            DictNode *node = dict_get_node_by_index(GET_DICT(globals), cache->index);
            if (node != NULL && is_obj_equals(node->key, cache->v.obj)) {
                MP_PUSH(node->val);
                PROFILE_END(cache);
            } else {
                cache->index = -1;
                cache->op = OP_LOAD_GLOBAL;
                PROFILE_END(cache);
                goto retry_op;
            }
            break;
        }
        case OP_STORE_GLOBAL_FAST: {
            PROFILE_START(cache);
            // TODO 需要对比一下key是否匹配,处理命中失败的情况
            GET_DICT(globals)->nodes[cache->index].val = MP_POP();
            PROFILE_END(cache);
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
            PROFILE_START(cache);
            v = MP_POP();
            k = MP_POP();
            x = MP_TOP();
            if (NOT_DICT(x)) {
                mp_raise("OP_DICT_SET: expect dict but see %o", x);
            }
            obj_set(x, k, v);
            PROFILE_END(cache);
            break;
        case OP_DICT: {
            MP_PUSH(dict_new());
            FRAME_CHECK_GC();
            break;
        }
        case OP_ADD: {
            // TODO 可以合并为 OP_ADD_TO_LOCAL
            PROFILE_START(cache); 
            MpObj* second = top-1;
            if (top->type == TYPE_NUM && second->type == TYPE_NUM) {
                second->value.num = top->value.num + second->value.num;
                top--;
            } else {
                *second = obj_add(*second, *top);
                top--;
            }
            PROFILE_END(cache);
            break;
        }
        // MP_OP(OP_ADD, obj_add)
        MP_OP(OP_SUB, obj_sub)
        MP_OP(OP_MUL, obj_mul)
        MP_OP(OP_DIV, obj_div)
        MP_OP(OP_MOD, obj_mod)
        
        case OP_GET: {
            PROFILE_START(cache);
            MpObj *key = top;
            MpObj *obj = top-1;
            top--;

            #ifdef FAST_GET

            if (IS_DICT(*obj)) {
                int idx = dict_get0(GET_DICT(*obj), *key);
                if (idx != -1) {
                    // 命中
                    *top = GET_DICT(*obj)->nodes[idx].val;
                    PROFILE_END(cache);

                    cache->op = OP_GET_FAST;
                    cache->index = idx;

                    break;
                }
            } 

            #endif
            
            *top = obj_get(*obj, *key);
            PROFILE_END(cache);
            break;
        }

        case OP_GET_FAST: {
            PROFILE_START(cache);
            MpObj *key = top;
            MpObj *obj = top-1;
            top--;

            if (IS_DICT(*obj)) {
                DictNode* node = dict_get_node_by_index(GET_DICT(*obj), cache->index);
                if (node != NULL && is_obj_equals(node->key, *key)) {
                    *top = node->val;
                    PROFILE_END(cache);
                    break;
                }
            }

            *top = obj_get(*obj, *key);

            cache->op = OP_GET;
            cache->index = -1;

            PROFILE_END(cache);
            break;
        }
        
        case OP_SLICE: {
            MpObj second = MP_POP();
            MpObj first  = MP_POP();
            *top = obj_slice(*top, first, second);
            break;
        }
        
        case OP_EQEQ: {
            PROFILE_START(cache);
            *(top-1) = number_obj(is_obj_equals(*(top-1), *top)); 
            top--; 
            PROFILE_END(cache);
            break; 
        }
        
        case OP_NOTEQ: { 
            *(top-1) = number_obj(!is_obj_equals(*(top-1), *top)); 
            top--; 
            break; 
        }
        
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
            *(top-1) = number_obj(mp_is_in(*(top-1), *top));
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

        case OP_SET:{
            PROFILE_START(cache);
            MpObj* key = top;
            MpObj* obj = top-1;
            MpObj* value = top-2;
            top-=3;

            if (obj->type == TYPE_DICT) {
                /* 没有命中缓存 */
                cache->index = dict_set0(obj->value.dict, *key, *value);
                cache->op = OP_SET_FAST;
            } else {
               obj_set(*obj, *key, *value);
            }

            PROFILE_END(cache);
            break;
        }

        case OP_SET_FAST: {
            PROFILE_START(cache);

            MpObj* key = top;
            MpObj* obj = top-1;
            MpObj* value = top-2;
            top-=3;

            if (IS_DICT(*obj)) {
                DictNode* node = dict_get_node_by_index(GET_DICT(*obj), cache->index);
                if (node != NULL && is_obj_equals(node->key, *key)) {
                    // printf("OP_SET_FAST: cache hit, index(%d)\n", cache->index);
                    node->val = *value; 
                    PROFILE_END(cache);
                    break;
                } 
            }

            // printf("OP_SET_FAST: cache miss, index(%d)\n", cache->index);
            // 缓存失效了，重新按照 OP_SET 执行
            cache->index = -1;
            cache->op = OP_SET;
            obj_set(*obj, *key, *value);
            break;
        }
        
        case OP_POP: {
            PROFILE_START(cache);
            top--;
            PROFILE_END(cache);
            break;
        }
        case OP_NEG:
            MP_TOP() = obj_neg(MP_TOP());
            break;

        // 函数调用
        case OP_CALL: {
            PROFILE_START(cache);
            // n是参数的个数
            int n = cache->v.ival;
            
            f->top = top;
            top -= n;

            // 当前的*top就是函数对象, top+1开始是参数
            // TODO top+1 can be optimized as locals
            arg_set_arguments(top + 1, n);
            MpObj func = MP_POP();

            PROFILE_END(cache);

            // 调用函数的时间不算到 OP_CALL的执行时间内
            MP_PUSH(OBJ_CALL_EX(func));

            // 恢复当前frame的值
            tm->frame = f;
            f->top = top;

            if (tm->allocated > tm->gc_threshold) {
                // gc_track(func);
                // mp_printf("gc full at %o\n", func);
                // printf("gc full at %s\n", obj_to_cstr(func));
                gc_full();
            }
            break;
        }

        // 尾递归调用
        case OP_TAILCALL: {
            int n = cache->v.ival;
            f->top = top;
            top -= n;
            MpObj* first_arg = top+1;
            arg_set_arguments(top+1, n);
            MpObj func = MP_POP();

            if (NOT_FUNC(func) && NOT_CLASS(func)) {
                mp_raise("OP_TAILCALL: invalid callable object %o", func);
            }

            // class也可以直接调用
            if (IS_FUNC(func) && GET_FUNCTION(func)->native == NULL) {
                /** tail call python function **/
                arg_start();
                int i = 0;
                for (i = 0; i < n; i++) {
                    arg_push(first_arg[i]);
                }

                assert(GET_FUNCTION(func)->resolved == 1);

                RESOLVE_METHOD_SELF(func);
                f->fnc = func;
                f->pc = GET_FUNCTION(func)->code;
                f->cache = GET_FUNCTION(func)->cache;
                f->maxlocals = func_get_max_locals(GET_FUNCTION(func));
                f->stack = f->locals + f->maxlocals;
                f->top = f->stack;
                *(f->top) = NONE_OBJECT;

                // clear locals
                for (i = 0; i < f->maxlocals; i++) {
                    f->locals[i] = NONE_OBJECT;
                }
                goto tailcall;
            } else {
                return OBJ_CALL_EX(func);
            }
            break;
        }
        case OP_APPLY: {
            f->top = top;
            MpObj args = MP_POP();
            mp_assert_type(args, TYPE_LIST, "mp_eval: OP_APPLY");
            arg_set_arguments(LIST_NODES(args), LIST_LEN(args));
            MpObj func = MP_POP();
            x = OBJ_CALL_EX(func);
            MP_PUSH(x);
            tm->frame = f;
            FRAME_CHECK_GC();
            break;
        }
        case OP_LOAD_PARAMS: {
            int parg = (cache->v.ival >> 8) & 0xff;
            int narg = cache->v.ival & 0xff;
            if (tm->arg_cnt < parg || tm->arg_cnt > parg + narg) {
                mp_raise("ArgError: parg=%d,narg=%d,given=%d", 
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
                locals[i] = arg_take_obj(func_name_cstr);
            }
            break;
        }
        case OP_LOAD_NARG: {
            int arg_index = cache->v.ival;
            MpObj list = list_new(tm->arg_cnt);
            while (arg_remains() > 0) {
                obj_append(list, arg_take_obj(func_name_cstr));
            }
            locals[arg_index] = list;
            break;
        }
        case OP_ITER: {
            *top = iter_new(*top);
            break;
        }
        case OP_NEXT: {
            MpObj *next = obj_next(*top);
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
            MpObj mod = GET_FUNCTION(cur_fnc)->mod;
            MpObj fnc = func_new(mod, NONE_OBJECT, NULL);
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
            MpObj class_name = cache->v.obj;
            MpObj clazz = class_new(class_name);
            dict_set0(GET_DICT(globals), class_name, clazz);
            break;
        }
        // 类定义结束
        case OP_CLASS_END: {
            break;
        }

        // 翻转堆栈
        // rotate stack
        case OP_ROT: {
            MpObj* left = top - cache->v.ival + 1;
            MpObj* right = top;
            for (; left < right; left++, right--) {
                MpObj temp = *left;
                *left = *right;
                *right = temp;
            }
            break;
        }
        case OP_UNPACK: {
            x = MP_POP();
            mp_assert_type(x, TYPE_LIST, "mp_eval: UNPACK");
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
            PROFILE_START(cache);
            if (is_true_obj(MP_TOP())) {
                PROFILE_END(cache);
                cache += cache->v.ival;
                continue;
            } else {
                PROFILE_END(cache);
            }
            break;
        }

        case OP_JUMP_ON_FALSE: {
            PROFILE_START(cache);
            if (!is_true_obj(MP_TOP())) {
                PROFILE_END(cache);
                cache += cache->v.ival;
                continue;
            } else {
                PROFILE_END(cache);
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
        case OP_DEF_END: {
           ret = NONE_OBJECT;
           goto end;
        }

        case OP_LOAD_EX: { 
            top = f->last_top; 
            MP_PUSH(tm->ex); 
            break; 
        }

        case OP_SETJUMP: { 
            f->last_top = top; 
            f->cache_jmp = cache + cache->v.ival;
            break; 
        }

        case OP_CLR_JUMP: { 
            f->jmp = NULL; 
            f->cache_jmp = NULL; 
            break;
        }

        case OP_LINE: {
            PROFILE_START(cache);
            f->lineno = cache->v.ival; 
            PROFILE_END(cache);
            break;
        }

        case OP_DEBUG: {
            #if 0
            MpObj fdebug = mp_get_global_by_cstr(globals, "__debug__");
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
                    obj_get_globals(f->fnc));
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
