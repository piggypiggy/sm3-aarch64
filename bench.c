/******************************************************************************
 *                                                                            *
 * Copyright 2020-2021 Meng-Shan Jiang                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *    http://www.apache.org/licenses/LICENSE-2.0                              *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 *                                                                            *
 *****************************************************************************/

#include "test.h"
#include "bench.h"

/* number of data used in each bench */
#define NUM 8

/*************************** sm3 ***************************/
#define MSGLEN 4096

typedef struct {
    u8 msg[MSGLEN];
    size_t msglen;
} SM3_DATA;

SM3_DATA *data;

static void bench_sm3_setup(void)
{
    int k;

    data = malloc(NUM * sizeof(SM3_DATA));
    /* random a, b, modulo */
    for (k = 0; k < NUM; k++) {
        random_string(data[k].msg, MSGLEN);
        data[k].msglen = MSGLEN;
    }
}

static void bench_sm3_run(int64_t N)
{
    int i, k;
    u8 hash[32];

    for(i = 0; i < N; i++) {
        k = (i + 1) % NUM;
        sm3_once(hash, data[k].msg, data[k].msglen);
    }
}

static void bench_sm3_clear(void)
{
    free(data);
}

static void _run_bench(int64_t N)
{
    double usec, avg_speed_MB, avg_speed_Gb;

    /* generate random data */
    bench_sm3_setup();

    BENCH_VARS;

    COUNTER_START();
    TIMER_START();

    /* run */
    bench_sm3_run(N);

    COUNTER_STOP();
    TIMER_STOP();

    bench_sm3_clear();

    usec = USEC();
    avg_speed_MB = (N * MSGLEN) / usec;
    avg_speed_Gb = 8 * avg_speed_MB / 1000;
    printf("cycles/block : %ld \n", (int64_t)(TICKS() / (N * MSGLEN / 64)));
    printf("总时间(ms)  \t  平均速度(MB/s)  \t 平均速度(Gb/s) \n");
    printf("%.4f  \t    %.4f       \t   %.4f \n\n", usec/1000, avg_speed_MB, avg_speed_Gb);
}

static void run_bench(char *name, int64_t N)
{
    printf("------------ %s ------------ \n", name);

    /* single thread */
    printf("singlethread N = %" PRId64 "\n\n", N);
    _run_bench(N);
}

int main(int argc, char **argv)
{
    int N;
    (void) argc;

    if (argc == 2) {
        if (memcmp(argv[1], "n=", 2) == 0 || memcmp(argv[1], "N=", 2) == 0)
            N = atoi(argv[1] + 2);
        else
            N = 10000;
    }
    else
        N = 10000;

    run_bench("sm3", N);

    return 0;
}
