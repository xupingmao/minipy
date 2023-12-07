/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2022-02-12 11:48:21
 * @LastEditors: xupingmao
 * @LastEditTime: 2023-12-07 23:38:45
 * @FilePath: /minipy/src/module/time.c
 * @Description: 描述
 */
#include "../include/mp.h"

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <sys/time.h>
#endif

static int64_t win32_get_nano_seconds() {
    /* FILETIME 包含一个 64 位值，表示自 1601 年 1 月 1 日 (UTC) 以来的 100 纳秒间隔数。*/
    FILETIME system_time;
    ULARGE_INTEGER large;

    GetSystemTimeAsFileTime(&system_time);
    large.u.LowPart = system_time.dwLowDateTime;
    large.u.HighPart = system_time.dwHighDateTime;
    /* 11,644,473,600,000,000,000: number of nanoseconds between
       the 1st january 1601 and the 1st january 1970 (369 years + 89 leap
       days). */
    return large.QuadPart * 100 - 11644473600000000000U;
}

int64_t time_get_milli_seconds() {
#ifdef _WIN32
    return win32_get_nano_seconds() / 1000000;
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