#include "../include/tm.h"

#ifndef _WIN32
    #include <sys/time.h>
#endif

Object bf_time_time() {
    #ifdef _WIN32
        SYSTEMTIME sTime;
        GetLocalTime(&sTime);
        double seconds = 0;
        seconds = (double) time(NULL) + (double) sTime.wMilliseconds / 1000.f;
        return tm_number(seconds);
    #else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tm_number((double)tv.tv_usec / 1000.f);
    #endif
}

Object bf_time_ctime() {
    time_t rawtime;
    time(&rawtime);
    char* ct = ctime(&rawtime); // ct ends with \n
    return string_alloc(ct, strlen(ct)-1);
}

void time_mod_init() {
    Object time_mod = dict_new();
    reg_mod_func(time_mod, "time",  bf_time_time);
    reg_mod_func(time_mod, "ctime", bf_time_ctime);
    dict_set_by_str(tm->modules, "time", time_mod);
}