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

typedef struct {
    /* input (byte) */
    char *in;
    /* hash (hex) */
    char *hash;
} SM3_TEST_VECTOR;

/* you can add more test vectors here :) */
static SM3_TEST_VECTOR sm3_test_vec[] =
{
    /* 1 */
    {
        "abc",
        "66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e0",
    },
    /* 2 */
    {
        "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd",
        "debe9ff92275b8a138604889c18e5a4d6fdb70e5387e5765293dcba39c0c5732",
    },
    /* 3 */
    {
        "Fed launches unlimited QE, but markets keep falling - as it happened",
        "45f5f7e2b7e7cdef2bc0766e193754e1875830b44e067d3bcc71304721875da8",
    },
    /* 4 */
    {
        "A purely peer-to-peer version of electronic cash would allow online payments to be sent directly from one party to another without going through a financial institution",
        "08c7dd4278a47bac588686088f2ea7c68ebdf297ca8a0d1fab08929c37fb68d5",
    },
    /* 5 */
    {
        "Bitcoin is a collection of of concepts and technologies that form the basis of a digital money ecosystem",
        "a3fd2167b9c72b3858f5a9441db93947146a7ee8597d042318da1d7fb0e42712",
    },
    /* 6 */
    {
        "My name is MengShan Jiang",
        "b7d7e3db6ff6adf93148a65463c5632f19fffd9cd6f4b9ea2a3aa19bd5f99cd1",
    },
};

int sm3_test_vector(void)
{
    int ret;
    size_t i;
    u8 h1[32];
    u8 h2[32];

    ret = SM3_ERR;
    for (i = 0; i < sizeof(sm3_test_vec) / sizeof(SM3_TEST_VECTOR); i++) {
        sm3_once(h1, (u8*)sm3_test_vec[i].in, strlen(sm3_test_vec[i].in));
        hex_to_u8(h2, (u8*)sm3_test_vec[i].hash, 64);
        if (memcmp(h1, h2, 32) != 0) {
            printf("sm3 test case %" PRId64 " failed\n", i+1);
            print_hex("hash = ", h1, 32);
            printf("hash should be:\n");
            print_hex("hash = ", h2, 32);
            goto end;
        }
    }

    printf("sm3 test vector passed \n");
    ret = SM3_OK;
end:
    return ret;
}

int sm3_test(void)
{
    int i, N;
    u8 msg[1024];
    size_t msglen;
    u8 d1[SM3_DIGEST_LENGTH], d2[SM3_DIGEST_LENGTH], d3[SM3_DIGEST_LENGTH];
    SM3_CTX sm3_ctx;

    N = 123456;
    for (i = 0; i < N; i++) {
        /* random message length */
        msglen = random_number() % 1024;
        /* random message */
        random_string(msg, msglen);
        /*  */
        sm3_once(d1, msg, msglen);
        sm3_once(d3, msg, msglen/2);

        sm3_init(&sm3_ctx);
        sm3_update(&sm3_ctx, msg, msglen/2);
        sm3_final(d2, &sm3_ctx);
        if (memcmp(d3, d2, SM3_DIGEST_LENGTH) != 0) {
            printf("sm3 test failed %d \n", i+1);
            return -1;
        }

        sm3_update(&sm3_ctx, msg + msglen/2, msglen - msglen/2);
        sm3_final(d2, &sm3_ctx);
        if (memcmp(d1, d2, SM3_DIGEST_LENGTH) != 0) {
            printf("sm3 test failed %d \n", i+1);
            return -1;
        }

        sm3_clean(&sm3_ctx);
    }

    printf("sm3 test passed \n");
    return 0;
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("-------- sm3 %s --------\n", sm3_get_impl_name());

    if (sm3_test_vector() == SM3_ERR)
        return -1;

    if (sm3_test() == SM3_ERR)
        return -1;

    return 0;
}
