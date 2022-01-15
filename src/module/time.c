#include "../include/mp.h"

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <sys/time.h>
#endif

int64_t time_get_milli_seconds() {
#ifdef _WIN32
    SYSTEMTIME st;
    GetLocalTime(&st);
    return st.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long sec = tv.tv_sec;
    long micro_sec = tv.tv_usec;
    return (int64_t)sec * 1000 + (int64_t)micro_sec/1000;
#endif
}

MpObj bf_time_time() {
    double ms = (double) time_get_milli_seconds();
    return number_obj(ms / 1000.f);
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