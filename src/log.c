
#ifndef _TM_LOG_
#define _TM_LOG_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#define TM_LOG_CACHE 0
#define TM_LOG_CALL  0

/** 
 * 1: info,
 * 2: warn,
 * else: error
 */
#define LOG_LEVEL 2


/**
 * 
 */
static void _log_time(FILE* fp) {
    time_t cur_time;
    time(&cur_time);
    char* t_str   = ctime(&cur_time);
    t_str[strlen(t_str)-1] = '\0';
    fprintf(fp, "%s,", t_str);
}

/**
 * @since 2016-11-13
 */
void log_info(char* fmt, ...) {
#if LOG_LEVEL <=1
    _log_time(stdout);
    
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    
    printf("\n");
#endif
}

/**
 * @since 2016-11-13
 */
void log_warn(char* fmt, ...) {
#if LOG_LEVEL <= 2
    _log_time(stdout);

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    
    printf("\n");
#endif
}

/**
 * @since 2016-11-13
 */
void log_error(char* fmt, ...) {
    _log_time(stdout);
    
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    
    printf("\n");
}

#if TM_LOG_CALL == 1
/**
 * @since 2016-11-28
 */
void tm_log_call(Object func) {
    Object name = GET_FUNCTION(func)->name;
    printf("%s(\n", GET_STR(name));
    int i;
    for (i = 0; i < tm->arg_cnt; i++) {
        tm_println(tm->arguments[i]);
    }
    printf(");\n");
}
#else
    #define tm_log_call(f) /*tm_log_call*/
#endif


#if TM_LOG_CACHE == 1
/**
 * @since 2016-11-27
 */
void tm_log_cache(TmCodeCache* cache) {
    int val = cache->v.ival;
    Object obj = cache->v.obj;

    switch(cache->op) {
        case OP_LINE:
            printf("line: %d\n", val);
            break;
        case OP_DEF:
            printf("def: %d\n", val);
            break;
        case OP_JUMP:
            printf("jmp: %d\n", val);
            break;
        case OP_UP_JUMP:
            printf("up_jmp: %d\n", val);
            break;
        case OP_CALL:
            printf("call: %d\n", val);
            break;
        case OP_RETURN:
            printf("return\n");
            break;
        case OP_STRING:
            printf("string: '%s'\n", GET_STR(cache->v.obj));
            break;
        case OP_LOAD_LOCAL:
            printf("loadl: %d\n", val);
            break;
        case OP_STORE_LOCAL:
            printf("storel: %d\n", val);
            break;
        case OP_LOAD_GLOBAL:
            printf("loadg: %s\n", GET_STR(obj));
            break;
        case OP_STORE_GLOBAL:
            printf("storeg: %s\n", GET_STR(obj));
            break;
        case OP_POP:
            printf("pop\n");
            break;
        case OP_NEXT:
            printf("next\n");
            break;
        case OP_SETJUMP:
            printf("setjmp: %d\n", val);
            break;
        case OP_CLR_JUMP:
            printf("clrjmp\n");
            break;
        case OP_LOAD_EX:
            printf("loadex\n");
            break;
        default:
            printf("tm_eval: %d\n", cache->op);
    }
}
#else
    #define tm_log_cache(c) /*tm_log_cache*/
#endif

#endif // _TM_LOG_