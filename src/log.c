/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/01/15 15:37:49
 */
#ifndef _MP_LOG_
#define _MP_LOG_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define MP_LOG_CACHE 0
#define MP_LOG_CALL  0

/** 
 * log level: with higher level, we keep more log
 * 0: no log
 * 1: error
 * 2: warn
 * 3: info
 * 4: debug
 */

#define LOG_LEVEL 2

#define kLogNone  0
#define kLogError 1
#define kLogWarn  2
#define kLogInfo  3
#define kLogDebug 4

// enum {
//     kLogNone = 0,
//     kLogError,
//     kLogWarn,
//     kLogInfo,
//     kLogDebug
// };

#define LOG_LEVEL 2

void log_init() {
    log_debug("log_init: logLevel=%d", LOG_LEVEL);
    log_debug("log_init: kLogDebug=%d", kLogDebug);
}



/**
 * log time
 */
static void _log_time(FILE* fp) {
    time_t cur_time;
    time(&cur_time);
    char* t_str   = ctime(&cur_time);
    t_str[strlen(t_str)-1] = '\0';
    fprintf(fp, "%s|", t_str);
}

/**
 * @since 2016-11-13
 */
void log_info(char* fmt, ...) {
#if LOG_LEVEL >= kLogInfo
    _log_time(stdout);
    printf("INFO|");
    
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    
    printf("\n");
#endif
}

void log_debug(char* fmt, ...) {
#if LOG_LEVEL >= kLogDebug
    _log_time(stdout);
    printf("DEBUG|");

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
#if LOG_LEVEL >= kLogWarn
    _log_time(stdout);
    printf("WARN|");

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
    printf("ERROR|");
    
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    
    printf("\n");
}

#if MP_LOG_CALL == 1
/**
 * @since 2016-11-28
 */
void mp_log_call(MpObj func) {
    MpObj name = GET_FUNCTION(func)->name;
    printf("%s(\n", GET_CSTR(name));
    int i;
    for (i = 0; i < tm->arg_cnt; i++) {
        mp_println(tm->arguments[i]);
    }
    printf(");\n");
}
#else
    #define mp_log_call(f) /*mp_log_call*/
#endif


#if MP_LOG_CACHE == 1
/**
 * @since 2016-11-27
 */
void mp_log_cache(MpCodeCache* cache) {
    int val = cache->v.ival;
    MpObj obj = cache->v.obj;

    tm->steps++;

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
            printf("string: '%s'\n", GET_CSTR(cache->v.obj));
            break;
        case OP_LOAD_LOCAL:
            printf("loadl: %d\n", val);
            break;
        case OP_STORE_LOCAL:
            printf("storel: %d\n", val);
            break;
        case OP_LOAD_GLOBAL:
            printf("loadg: %s\n", GET_CSTR(obj));
            break;
        case OP_STORE_GLOBAL:
            printf("storeg: %s\n", GET_CSTR(obj));
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
            printf("mp_eval: %d\n", cache->op);
    }
}
#else
    #define mp_log_cache(c) /*mp_log_cache*/
#endif

#endif // _MP_LOG_