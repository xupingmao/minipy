#ifndef _MP_LOG_H
#define _MP_LOG_H

#if 0
    #define PRINT_MP_GC_INFO_START() int _gc_old = tm->allocated;
    #define PRINT_MP_GC_INFO_END(str, addr) \
        printf("free %s at 0x%p, %d => %d, ", str, addr, _gc_old, tm->allocated);
    #else
    #define PRINT_MP_GC_INFO_START()
    #define PRINT_MP_GC_INFO_END(str, addr)

    #define GC_LOG_START(ptr, desc)
    #define GC_LOG_END(ptr, desc)
#endif

/* debug and log functions */
#define DEBUG(msg) \
    printf("--DEBUG %s\n", msg);

#define DEBUG2(msg1, msg2) \
    printf("--DEBUG %s %s\n", msg1, msg2);

#define MP_LOG_INFO printf

void log_init();
void log_destroy();
void log_debug(char* fmt, ...);
void log_info(char* fmt, ...);
void log_warn(char* fmt, ...);
void log_error(char* fmt, ...);
void log_stderr(char* fmt, ...);

#endif