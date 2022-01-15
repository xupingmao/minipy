#ifndef _MP_LOG_H

/* debug and log functions */
#define DEBUG(msg) \
    printf("--DEBUG %s\n", msg);

#define DEBUG2(msg1, msg2) \
    printf("--DEBUG %s %s\n", msg1, msg2);

#define LOG_INFO printf

void log_init();
void log_debug(char* fmt, ...);
void log_info(char* fmt, ...);
void log_warn(char* fmt, ...);
void log_error(char* fmt, ...);

#endif