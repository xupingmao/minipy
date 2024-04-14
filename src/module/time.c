/*
 * @Author: xupingmao 578749341@qq.com
 * @Date: 2024-04-14 12:29:46
 * @LastEditors: xupingmao 578749341@qq.com
 * @LastEditTime: 2024-04-14 12:38:12
 * @FilePath: /minipy/src/module/time.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2022-02-12 11:48:21
 * @LastEditors: xupingmao 578749341@qq.com
 * @LastEditTime: 2024-04-14 12:35:45
 * @FilePath: /minipy/src/module/time.c
 * @Description: 描述
 */
#include "../include/mp.h"

#ifdef _WIN32
    #include "time_win32.c"
#else
    #include "time_posix.c"
#endif


static MpObj bf_time_time() {
    double ms = (double) time_get_milli_seconds();
    return number_obj(ms / 1000.f);
}

static MpObj bf_time_clock() {
    return number_obj((double)clock()/CLOCKS_PER_SEC);
}


static MpObj bf_time_ctime() {
    time_t rawtime;
    time(&rawtime);
    char* ct = ctime(&rawtime); // ct ends with \n
    return string_alloc(ct, strlen(ct)-1);
}

static MpObj bf_time_sleep() {
    int i = 0;
    int t = arg_take_int("sleep");
#ifdef _WIN32
    Sleep(t);
#else
    sleep(t);
#endif
    return NONE_OBJECT;
}

static MpObj bf_time_asctime() {
    time_t rawtime;
    struct tm* timeinfo;
    
    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    char* ct = asctime(timeinfo); // ct ends with \n
    return string_alloc(ct, strlen(ct)-1);
}

void mp_time_init() {
    MpObj time_mod = dict_new();
    reg_mod_func(time_mod, "time",  bf_time_time);
    reg_mod_func(time_mod, "clock", bf_time_clock);
    reg_mod_func(time_mod, "ctime", bf_time_ctime);
    reg_mod_func(time_mod, "sleep", bf_time_sleep);
    reg_mod_func(time_mod, "asctime", bf_time_asctime);
    
    reg_mod("time", time_mod);
}