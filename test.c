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

/* 0->'0', 1->'1', ... , 15->'F' */
static const u8 ascii_table[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
};

static const u8 inv_ascii_table[128] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
       0,    1,    2,    3,    4,    5,    6,    7,
       8,    9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff,   10,   11,   12,   13,   14,   15, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff,   10,   11,   12,   13,   14,   15, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

int u8_to_hex(u8 *out, const u8 *in, size_t inlen)
{
    size_t i;

    if (out == NULL)
        return -1;

    for (i = 0; i < inlen; i++) {
        out[0] = ascii_table[in[i] >> 4];
        out[1] = ascii_table[in[i] & 0xf];
        out += 2;
    }

    return 0;
}

int hex_to_u8(u8 *out, const u8 *in, size_t inlen)
{
    size_t i;

    if (out == NULL)
        return -1;

    i = 0;
    if (inlen % 2 == 1) {
        out[0] = inv_ascii_table[in[i]];
        if (out[0] == 0xff)
            return -1;
        out++;
        i++;
    }

    for (; i < inlen; i += 2) {
        out[0] = (inv_ascii_table[in[i]] << 4) | inv_ascii_table[in[i+1]];
        // if (out[0] == 0xff)
        //     return FP256_ERR;
        out++;
    }
    return 0;
}

void print_hex(const char *desp, const u8 *s, size_t slen)
{
    size_t i;

    for(i = 0; i < strlen(desp); i++)
        printf("%c", desp[i]);

    u8 *hex = (u8*)malloc(2*slen);
    u8_to_hex(hex, s, slen);
    for(i = 0; i < 2*slen; i++)
        printf("%c", hex[i]);
    printf("\n");
    free(hex);
}

/* bad rng, but good for test */
void random_string(u8 *s, size_t len)
{
    static int en = 0;
    size_t i;

    srand((unsigned)time(NULL) + en);
    for (i = 0; i < len; i++)
        s[len] = rand() % 256;
    en++;
}

size_t random_number(void)
{
    static int en = 0;
    srand((unsigned)time(NULL) + en);
    en++;
    return (size_t)rand();
}
