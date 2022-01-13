#include "../include/mp.h"

#ifndef _WIN32
    #include <sys/time.h>
#endif

MpObj bf_time_time() {
    #ifdef _WIN32
        // FILETIME system_time;
        /* Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC). */
        // GetSystemTimeAsFileTime(&system_time);
        // double seconds = 0;
        // DWORD lowDateTime  = system_time.dwLowDateTime;
        // DWORD highDateTime = system_time.dwHighDateTime;
        // int64_t ms;
        // ms = lowDateTime;
        // ms += (((int64_t)highDateTime) << 32);
        
        // SYSTEMTIME stime;
        // GetLocalTime(&stime);
        // stime.wMilliseconds
        
        // just return seconds
        return number_obj(time(NULL));
    #else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return number_obj((double)tv.tv_usec / 1000000.f);
    #endif
}

MpObj bf_time_clock() {
    return number_obj((double)clock()/CLOCKS_PER_SEC);
}


MpObj bf_time_ctime() {
    time_t rawtime;
    time(&rawtime);
    char* ct = ctime(&rawtime); // ct ends with \n
    return string_alloc(ct, strlen(ct)-1);
}

MpObj bf_time_sleep() {
    int i = 0;
    int t = arg_take_int("sleep");
#ifdef _WIN32
    Sleep(t);
#else
    sleep(t);
#endif
    return NONE_OBJECT;
}

MpObj bf_time_asctime() {
    time_t rawtime;
    struct tm* timeinfo;
    
    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    char* ct = asctime(timeinfo); // ct ends with \n
    return string_alloc(ct, strlen(ct)-1);
}

void time_mod_init() {
    MpObj time_mod = dict_new();
    reg_mod_func(time_mod, "time",  bf_time_time);
    reg_mod_func(time_mod, "clock", bf_time_clock);
    reg_mod_func(time_mod, "ctime", bf_time_ctime);
    reg_mod_func(time_mod, "sleep", bf_time_sleep);
    reg_mod_func(time_mod, "asctime", bf_time_asctime);
    
    reg_mod("time", time_mod);
}