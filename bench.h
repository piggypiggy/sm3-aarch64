#pragma once

#include "config.h"

#if defined(__GNUC__) && defined(ARCH_AARCH64)
#include "sys/time.h"
#define BENCH_VARS               \
    u64 start_ticks, end_ticks;      \
    int64_t start_time, end_time                                                              

#define COUNTER_START()          \
  __asm__ volatile               \
    ("isb  \n"                   \
     "mrs %0, cntvct_el0  \n"    \
     : "=r" (start_ticks))

#define COUNTER_STOP()           \
  __asm__ volatile               \
    ("isb  \n"                   \
     "mrs %0, cntvct_el0  \n"    \
     : "=r" (end_ticks))

static inline int64_t gettime_i64()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000UL + tv.tv_usec;
}

#define TIMER_START()            \
    start_time = gettime_i64()

#define TIMER_STOP()             \
    end_time = gettime_i64()

#define TICKS() (end_ticks - start_ticks)
#define USEC() (end_time - start_time)
#else

#error "timer not implemented."

#endif
