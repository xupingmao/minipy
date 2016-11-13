
#ifndef _TM_LOG_
#define _TM_LOG_

#include <stdarg.h>
#include <time.h>
#include <string.h>


/** 
 * 1: info,
 * 2: warn,
 * else: error
 */
#define LOG_LEVEL 2

/** copied from gc.c */
enum {
    GC_INIT,
    GC_WORKING,
    GC_STATE_DESTROY
} GcState;

#ifdef GC_DEBUG
// logs can redirected to file
#define WRITE_LOG_FILE 0

static char LOG_BUF[1024 * 10]; // 20K buffer

enum {
    LEVEL_DEBUG,
    LEVEL_INFO, 
    LEVEL_ERROR,
    LEVEL_FATAL,
} LogLevel;

// debug, info, error, fatal
static int gc_log_level = LEVEL_ERROR;
static long long gc_log_count = 0;


int check_log_level(int level) {
    if (gc_log_level == LEVEL_FATAL) {
        return 1;
    } else if (gc_log_level == LEVEL_ERROR) {
        return level == LEVEL_ERROR || level == LEVEL_FATAL;
    } else if (gc_log_level == LEVEL_INFO) {
        return level != LEVEL_DEBUG;
    } else if (gc_log_level == LEVEL_DEBUG) {
        return level == LEVEL_DEBUG;
    }
    return 0;
}

void DEBUG_INIT() {
    if (WRITE_LOG_FILE) {
        FILE* fp = fopen("tm.log", "w");
        // fputs("*****LOG INIT*****\n", fp);
        fclose(fp);
    }

    /** init debug map */
    debug_init();
}

void DEBUG_MALLOC(void* ptr) {
    debug_malloc(ptr);
}

void DEBUG_FREE(void*ptr) {
    debug_free(ptr);
}

void DEBUG_FREE2(void*ptr, Object o) {
    debug_free2(ptr, o);
}

 /**
  * logging service
  * suggested format
  * TIME|OPT|CURRENT_SIZE|NEW_SIZE|ADDR
  */
void LOG(int level, char* fmt, ...) {
    va_list ap;
    time_t cur_time;
    char *buffer   = LOG_BUF;
    int BUF_SIZE   = sizeof(LOG_BUF);
    int FLUSH_SIZE = BUF_SIZE / 2;
    char temp_buf[1024];

    if (!check_log_level(level)) {
        return;
    }

    memset(temp_buf, 0, sizeof(temp_buf));
    va_start(ap, fmt);

    time(&cur_time); // init time
    char* t_str   = ctime(&cur_time);
    int t_str_len = strlen(t_str);
    t_str[t_str_len-1] = 0; // remove \n

    sprintf (temp_buf, "%s,", t_str); // print time
    strcat(buffer, temp_buf);

    vsprintf(temp_buf, fmt, ap);

    char count_buf[20];
    sprintf(count_buf, ",%d", gc_log_count);
    gc_log_count += 1;

    strcat(temp_buf, count_buf);
    strcat(temp_buf, "\n");

    fputs(temp_buf, stdout); // print to stdout

    strcat(buffer, temp_buf);

    va_end(ap);

    if (WRITE_LOG_FILE && strlen(buffer) >= FLUSH_SIZE) {
        FILE* fp = fopen("tm.log", "a");
        fputs(buffer, fp);
        fclose(fp);
        memset(buffer, 0, BUF_SIZE);
    }
}

/**
 * flush log
 * @since 2016-08-16
 */
void LOG_END() {

    if (WRITE_LOG_FILE) {
        FILE* fp = fopen("tm.log", "a");
        fputs(LOG_BUF, fp);
        fclose(fp);
        memset(LOG_BUF, 0, sizeof(LOG_BUF));
    }

    debug_destroy();
}
#else
    #define DEBUG_INIT() /* DEBUG_INIT */
    #define LOG(level, fmt, oldsize, newsize, addr) /* LOG */
    #define LOG_END() /* LOG END */
    #define DEBUG_MALLOC(ptr) /* DEBUG_MALLOC */
    #define DEBUG_FREE(ptr) /* DEBUG_FREE */
    #define DEBUG_FREE2(ptr,o) /* DEBUG_FREE2 */
#endif
/** copied from gc */

/**
 * 
 */
void _log_time(FILE* fp) {
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

#endif // _TM_LOG_