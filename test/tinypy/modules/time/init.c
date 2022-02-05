#include <ctype.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <sys/time.h>
#endif

typedef long long int64_t;

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

static tp_obj bf_time_time(TP) {
    double ms = (double) time_get_milli_seconds();
    return tp_number(ms / 1000.f);
}

static tp_obj bf_time_clock() {
    return tp_number((double)clock()/CLOCKS_PER_SEC);
}

/*
 * time_mod_init()
 *
 * time module initialization function
 */
void time_init(TP)
{
    /*
     * module dict for time
     */
    tp_obj time_mod = tp_dict(tp);

    /*
     * bind functions to random module
     */
    tp_set(tp, time_mod, tp_string("time"),  tp_fnc(tp, bf_time_time));
    tp_set(tp, time_mod, tp_string("clock"), tp_fnc(tp, bf_time_clock));

    /*
     * bind usual distribution random variable generator
     */
    /* tp_set(tp, random_mod, tp_string("uniform"), tp_fnc(tp, random_uniform));*/

    /*
     * bind special attributes to time module
     */
    tp_set(tp, time_mod, tp_string("__doc__"),  tp_string("Time."));
    tp_set(tp, time_mod, tp_string("__name__"), tp_string("time"));
    tp_set(tp, time_mod, tp_string("__file__"), tp_string(__FILE__));

    /*
     * bind random module to tinypy modules[]
     */
    tp_set(tp, tp->modules, tp_string("time"), time_mod);
}
