/**
 date : 2014-9-2
 
 2015-6-16: interpreter for tinyvm bytecode.
 **/

#include "include/interp.h"
#include "include/exception.h"
#include "include/function.h"

#define INTERP_DB 0

Object callFunction2(Object func) {
    if (NOT_FUNC(func) || IS_NATIVE(func)) {
        return UNDEF;
    }
    resolveMethodSelf(func);
    TmFrame* f = pushFrame(func);
    reCall:
    if (setjmp(f->buf)==0) {
        return tmEval(f);
    } else {
        f = tm->frame;
        /* handle exception in this frame */
        if (f->jmp != NULL) {
            f->pc = f->jmp;
            f->jmp = NULL;
            goto reCall;
        /* there is no handler, throw to last frame */
        } else {
            pushException(f);
            tm->frame--;
            return UNDEF;
        }
    }
}

Object callFunction(Object func) {
    Object ret;
    if (IS_FUNC(func)) {
        resolveMethodSelf(func);
        /* call native */
        if (GET_FUNCTION(func)->native != NULL) {
            return GET_FUNCTION(func)->native();
        } else {
            TmFrame* f = pushFrame(func);
            reCall:
            if (setjmp(f->buf)==0) {
                return tmEval(f);
            } else {
                f = tm->frame;
                /* handle exception in this frame */
                if (f->jmp != NULL) {
                    f->pc = f->jmp;
                    f->jmp = NULL;
                    goto reCall;
                /* there is no handler, throw to last frame */
                } else {
                    pushException(f);
                    tm->frame--;
                    longjmp(tm->frame->buf, 1);
                }
            }
        }
    } else if (IS_DICT(func)) {
        ret = classNew(func);
        Object *_fnc = dictGetByStr(ret, "__init__");
        if (_fnc != NULL) {
            callFunction(*_fnc);
        }
        return ret;
    }
    tmRaise("callFunction:invalid object %o", (func));
    return NONE_OBJECT;
}


void popFrame() {
	tm->frame --;
}

#define TM_OP(OP_CODE, OP_FUNC) case OP_CODE: \
    *(top-1) = OP_FUNC(*(top-1), *top);--top;\
    break;
    
#define FRAME_CHECK_GC()  \
f->top = top; \
if (tm->allocated > tm->gcThreshold) {   \
	gcFull();                            \
}

TmFrame* pushFrame(Object fnc) {
    /* make extra space for self in method call */
    Object *top = tm->frame->top + 2;
	tm->frame ++ ;
	TmFrame* f = tm->frame;

    /* check oprand stack */
	if (top >= tm->stack + STACK_SIZE) {
		popFrame();
		tmRaise("tm_eval: stack overflow");
	}
    
	/* check frame stack*/
	if (tm->frame >= tm->frames + FRAMES_COUNT-1) {
		popFrame();
		tmRaise("tm_eval: frame overflow");
	}

	f->pc = GET_FUNCTION(fnc)->code;

	f->locals = top;
	f->maxlocals = getFunctionMaxLocals(GET_FUNCTION(fnc));
	f->stack = f->locals + f->maxlocals;
    f->top = f->stack;
	f->fnc = fnc;
 
	int i;for(i = 0; i < f->maxlocals; i++) {
		f->locals[i] = NONE_OBJECT;
	}
    *(f->top) = NONE_OBJECT;
	f->jmp = NULL;
	return f;
}


/** 
** evaluate byte code.
** @param f: Frame
** @return evaluated value.
*/
Object tmEval(TmFrame* f) {
	Object* locals = f->locals;
	Object* top = f->stack;
	Object cur_fnc = f->fnc;
	Object globals = getGlobals(cur_fnc);
	unsigned char* pc = f->pc;

	Object x, k, v;
	Object ret = NONE_OBJECT;
	int i;

    #if INTERP_DB
        printf("File \"%s\": enter function %s\n",getFuncFileSz(cur_fnc), getFuncNameSz(cur_fnc));
    #endif
	while (1) {
		i = (pc[1] << 8) | pc[2];
        
        #if INTERP_DB
            printf("%30s%2d: %d frame = %d, top = %d\n","", pc[0], i, tm->cur, (int) (top - f->stack));
        #endif    
		switch (pc[0]) {

		case NEW_NUMBER: {
			double d = atof((char*)pc + 3);
			pc += i;
			v = newNumber(d);
			/* APPEND(tm->constants,v);*/
            dictSet(tm->constants, v, NONE_OBJECT);
			break;
		}

		case NEW_STRING: {
			v = newString0((char*)pc + 3, i);
			pc += i;
			/* APPEND(tm->constants,v); */
            dictSet(tm->constants, v, NONE_OBJECT);
			break;
		}

		case LOAD_CONSTANT: {
			TM_PUSH(GET_CONST(i));
            /* predict , eg. SET, GET etc. */
            /*
            pc += 3;
            if (pc[0] == SET) {
                // need to check whether is a dict. 
            } else if(pc[0] == GET) {
                
            }
            */
			break;
		}
        
        case LOAD_NONE: {
            TM_PUSH(NONE_OBJECT);
            break;
        }

        case LOAD_GLOBALS: {
        	TM_PUSH(globals);
        	break;
        }

		case LOAD_LOCAL: {
			TM_PUSH(locals[i]);
			break;
		}

		case STORE_LOCAL:
			locals[i] = TM_POP();
			break;

		case LOAD_GLOBAL: {
            /* tmPrintf("load global %o\n", GET_CONST(i)); */
			int idx = getAttr(GET_DICT(globals), i);
			if (idx == -1) {
				idx = getAttr(GET_DICT(tm->builtins), i);
				if (idx == -1) {
					tmRaise("NameError: name %o is not defined", GET_CONST(i));
				}
				TM_PUSH(GET_DICT(tm->builtins)->nodes[idx].val);
			} else {
				TM_PUSH(GET_DICT(globals)->nodes[idx].val);
                pc[0] = FAST_LD_GLO;
                pc[1] = (idx>>8) & 0xff;
                pc[2] = idx & 0xff;
			}
            /*
			k = GET_CONST(i);
			DictNode* node = DictGetNode(GET_DICT(globals), k);
			if (node == NULL) {
				node = DictGetNode(GET_DICT(tm->builtins), k);
				if (node == NULL) {
					tmRaise("NameError: name %o is not defined", k);
				}
				v = node->val;
			} else {
				v = node->val;
			}
			TM_PUSH(v);
            */
			break;
		}
		case STORE_GLOBAL: {
			x = TM_POP();
            /* tmPrintf("store global %o\n", GET_CONST(i)); */
            int idx = setAttr(GET_DICT(globals), i, x);
			/* tmSet(globals, GET_CONST(i), x); */
            pc[0] = FAST_ST_GLO;
            pc[1] = (idx >> 8) & 0xff;
            pc[2] = idx & 0xff;
			break;
		}
        case FAST_LD_GLO: {
            TM_PUSH(GET_DICT(globals)->nodes[i].val);
            break;
        }
        case FAST_ST_GLO: {
            GET_DICT(globals)->nodes[i].val = TM_POP();
            break;
        }
		case LIST: {
			TM_PUSH(newList(2));
			FRAME_CHECK_GC();
			break;
		}
		case LIST_APPEND:
			v = TM_POP();
			x = TM_TOP();
			tmAssertType(x, TYPE_LIST, "tmEval: LIST_APPEND");
			_listAppend(GET_LIST(x), v);
			break;
		case DICT_SET:
			v = TM_POP();
			k = TM_POP();
			x = TM_TOP();
			tmAssertType(x, TYPE_DICT, "tmEval: DICT_SET");
			tmSet(x, k, v);
			break;
		case DICT: {
			TM_PUSH(newDict());
			FRAME_CHECK_GC();
			break;
		}
		TM_OP(ADD, tmAdd)
		TM_OP(SUB, tmSub)
		TM_OP(MUL, tmMul)
		TM_OP(DIV, tmDiv)
		TM_OP(MOD, tmMod)
		TM_OP(GET, tmGet)
		case EQEQ: {
            *(top-1) = newNumber(tmEquals(*(top-1), *top));
            top--;
            break;
        }
        case NOTEQ: {
            *(top-1) = newNumber(!tmEquals(*(top-1), *top));
            top--;
            break;
        }
        case LT: {
            *(top-1) = newNumber(tmCmp(*(top-1), *top)<0);
            top--;
            break;
        }
        case LTEQ: {
            *(top-1) = newNumber(tmCmp(*(top-1), *top)<=0);
            top--;
            break;
        }
        case GT: {
            *(top-1) = newNumber(tmCmp(*(top-1), *top)>0);
            top--;
            break;
        }
        case GTEQ: {
            *(top-1) = newNumber(tmCmp(*(top-1), *top)>=0);
            top--;
            break;
        }
        case OP_IN: {
            *(top-1) = newNumber(bTmHas(*top, *(top-1)));
            top--;
            break;
        }
        case AND: {
            *(top-1) = newNumber(tmBool(*(top-1)) && tmBool(*top));
            top--;
            break;
        }
        case OR: {
            *(top-1) = newNumber(tmBool(*(top-1)) || tmBool(*top));
            top--;
            break;
        }
        case NOT:{
            *top = newNumber(!tmBool(*top));
            break;
        }
		case SET:
			k = TM_POP();
			x = TM_POP();
			v = TM_POP();
            #if INTERP_DB
                tmPrintf("Self %o, Key %o, Val %o\n", x, k, v);
            #endif
			tmSet(x, k, v);
			break;
		case POP: {
			top--;
			break;
		}
		case NEG:
			TM_TOP() = tmNeg(TM_TOP());
			break;
		case CALL: {
            f->top = top;
			top -= i;
			tmSetArguments(top + 1, i);
			Object func = TM_POP();
            #if INTERP_DB
                printf("call %s\n", getFuncNameSz(func));
            #endif
            TM_PUSH(callFunction(func));
            tm->frame = f;
            FRAME_CHECK_GC();
            break;
		}
		case LOAD_PARAMS: {
            // tmPrintf("load %d params\n", tm->argumentsCount);
			for(i = 0; i < tm->argumentsCount; i++){
				locals[i] = tm->arguments[i];
			}
			break;
		}
        case TM_NARG: {
            Object list = newList(tm->argumentsCount);
            for(i = 0; i < tm->argumentsCount; i++) {
                APPEND(list, tm->arguments[i]);
            }
            locals[0] = list;
            break;
        }
		case ITER_NEW: {
			*top = iterNew(*top);
			break;
		}
		case TM_NEXT: {
			Object *next = tmNext(*top);
			if (next != NULL) {
				TM_PUSH(*next);
				break;
			} else {
				pc += i * 3;
				continue;
			}
			break;
		}
		case TM_DEF: {
            Object mod = GET_FUNCTION(cur_fnc)->mod;
            CodeCheckResult rs = resolveCode(GET_MODULE(mod), pc + 3, 1);
            Object fnc = newFunction(mod, NONE_OBJECT, NULL);
            GET_FUNCTION(fnc)->code = rs.code;
            GET_FUNCTION(fnc)->maxlocals = rs.maxlocals;
            GET_FUNCTION(fnc)->maxstack = rs.maxstack;
			GET_FUNCTION_NAME(fnc) = GET_CONST(i);
			pc += rs.len;
			TM_PUSH(fnc);
			break;
		}
		case RETURN: {
            ret = TM_POP();
            goto end;
		}
		case TM_ROT: {
			int half = i / 2;
			int j;
			for (j = 0; j < half; j++) {
				Object temp = *(top - j);
				*(top-j) = *(top - i + j + 1);
				*(top-i+j+1) = temp;
			}
            break;
		}
		case TM_UNARRAY: {
			x = TM_POP();
			tmAssertType(x, TYPE_LIST, "tmEval:TM_UNARRAY");
			int j;
			for(j = LIST_LEN(x)-1; j >= 0; j--) {
				TM_PUSH(LIST_GET(x, j));
			}
			break;
		}

		case TM_DEL: {
			k = TM_POP();
			x = TM_POP();
			tmDel(x, k);
			break;
		}

		case POP_JUMP_ON_FALSE: {
			if (!tmBool(TM_POP())) {
				pc += i * 3;
				continue;
			}
			break;
		}

		case JUMP_ON_TRUE: {
			if (tmBool(TM_TOP())) {
				pc += i * 3;
				continue;
			}
			break;
		}

		case JUMP_ON_FALSE: {
			if (!tmBool(TM_TOP())) {
				pc += i * 3;
				continue;
			}
			break;
		}

		case UP_JUMP:
			pc -= i * 3;
			continue;

		case JUMP:
			pc += i * 3;
			continue;

        case TM_EOP:
		case TM_EOF: {
           ret = NONE_OBJECT;
           goto end;
		}

		case LOAD_EX: {
			TM_PUSH(tm->ex);
			break;
		}

		case SETJUMP: {
            f->jmp = pc + i * 3;
            /*
            buf.prev = tm->buf;
            tm->buf = &buf;
            if (setjmp(tm->buf->b) == 0) {
            } else {
                f = tm->frame;
                if (tm->frame->jmp != NULL) {
                    f = tm->frame;
                    f->pc = f->jmp;
                    f->jmp = NULL;
                    UPDATE_ENV();
                    continue;
                } else {
                    pushException(f);
                    ret = UNDEF;
                    goto end;
                }
            }*/
			break;
		}

		case TM_DEBUG: {
			Object *debugFunc = getBuiltin("__debug__");
			if (debugFunc == NULL) {
				tmRaise("__debug__ is not defined");
			} else {
				argStart();
				pushArg(newNumber((long)pc));
                pushArg(globals);
                /* how to pass locals? */
                /* make a list in frame ? store arguments as well as locals*/
				callFunction(*debugFunc);
                continue;
			}			
			break;
		}

		default:
			tmRaise("BAD INSTRUCTION, %d\n  globals() = \n%o", pc[0],
					GET_FUNCTION_GLOBALS(f->fnc));
			goto end;
		}

		pc += 3;
	}

	end:
    /*
	if (top != f->stack) {
        tmRaise("tmEval: operand stack overflow");
	}*/
    popFrame();
	return ret;
}
