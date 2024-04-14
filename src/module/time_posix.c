/*
 * @Author: xupingmao 578749341@qq.com
 * @Date: 2024-04-14 12:36:41
 * @LastEditors: xupingmao 578749341@qq.com
 * @LastEditTime: 2024-04-14 12:39:46
 * @FilePath: /minipy/src/module/time_posix.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include <stdlib.h>
#include <sys/time.h>

int64_t time_get_milli_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long sec = tv.tv_sec;
    long micro_sec = tv.tv_usec;
    return (int64_t)sec * 1000 + (int64_t)micro_sec/1000;
}

