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

void time_mod_init() {
    Object time_mod = dict_new();
    reg_mod_func(time_mod, "time", bf_time_time);

    dict_set_by_str(tm->modules, "time", time_mod);
}