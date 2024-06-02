#include <Windows.h>

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
    return win32_get_nano_seconds() / 1000000;
}