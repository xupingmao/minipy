#include "include/mp.h"

#ifndef MP_PROFILE
    #define MP_PROFILE 0
#endif

#if MP_PROFILE
    #define PROFILE_START(cache) profile_start_impl(cache);
    #define PROFILE_END(cache)   profile_end_impl(cache);
    #define PROFILE_INIT()       profile_init();
    #define PROFILE_NULL_START() profile_start_impl(&profile_code);
    #define PROFILE_NULL_END()   profile_end_impl(&profile_code);
#else
    #define PROFILE_START(cache) if(0){};
    #define PROFILE_END(cache) if(0){};
    #define PROFILE_INIT() if(0){};
    #define PROFILE_NULL_START() if(0){};
    #define PROFILE_NULL_END()   if(0){};
#endif

typedef struct _ProfileInfo {
    char op_code;
    // 全部的时间，包括统计指令本身的耗时
    int64_t total_time;
    // 真实的时间，减去统计指令空转的平均耗时
    int64_t real_time;
    int64_t times;
} ProfileInfo;

#define PROFILE_SIZE 256
static ProfileInfo profiles_[PROFILE_SIZE];
static MpCodeCache profile_code;

#if MP_PROFILE
void profile_init() {
    for (int i = 0; i < PROFILE_SIZE; i++) {
        profiles_[i].op_code = i;
        profiles_[i].total_time = 0;
        profiles_[i].real_time  = 0;
        profiles_[i].times = 0;
    }

    profile_code.op = OP_PROFILE;
}

void profile_start_impl(MpCodeCache* cache) {
    cache->start_time = time_get_milli_seconds();
}

void profile_end_impl(MpCodeCache* cache) {
    int64_t cost_time = time_get_milli_seconds() - cache->start_time;
    cache->total_time += cost_time;
    cache->times += 1;

    profiles_[cache->op].total_time += cost_time;
    profiles_[cache->op].times ++;
}

static void print_line(int cols) {
    for (int i = 0; i < cols; i++) {
        printf("-");
    }
    printf("\n");
}

int profile_compare(const void* left, const void* right) {
    const ProfileInfo* a = (const ProfileInfo*) left;
    const ProfileInfo* b = (const ProfileInfo*) right;
    return b->total_time - a->total_time;
}

double profile_null_avg_time() {
    ProfileInfo null_info = profiles_[OP_PROFILE];

    if (null_info.times <= 0) {
        return 0.0;
    }

    return (double) null_info.total_time / null_info.times;
}

void profile_compute_real_time() {
    double null_avg_time = profile_null_avg_time();

    for (int i = 0; i < PROFILE_SIZE; i++) {
        ProfileInfo info = profiles_[i];
        if (info.times > 0) {
            double real_time = (double)info.total_time - null_avg_time * info.times;
            info.real_time = MAX(real_time, 0);
        }
    }
}

void profile_print_detail() {
    printf("\n\n== Minipy Profile Stat\n");
    print_line(60);
    printf("%-20s %10s %10s %s %s\n", "op_name", 
        "total_time", "times", "avg_time", "real_avg");
    print_line(60);

    double null_avg_time = profile_null_avg_time();
    profile_compute_real_time();

    qsort(profiles_, PROFILE_SIZE, sizeof(ProfileInfo), profile_compare);

    for (int i = 0; i < PROFILE_SIZE; i++) {
        ProfileInfo info = profiles_[i];
        if (info.times > 0) {
            double avg_time = (double) info.total_time / info.times;
            double real_avg = MAX(avg_time - null_avg_time, 0);
            const char* op_name = inst_get_name_by_code(info.op_code);
            printf("%-20s %10ld %10ld %lf %lf\n", op_name, 
                info.total_time, info.times, avg_time, real_avg);
        }
    }
}

#endif
