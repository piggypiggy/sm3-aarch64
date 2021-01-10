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

#include <sm3.h>
#include "config.h"

u32 ll_bswap4(const u32 a);
u64 ll_bswap8(const u64 a);
void sm3_compress_neon(u32 digest[8], const u8 *buf, u64 nb);

static const u8 PAD[64] = { 
    0x80, 0, 0, 0, 0, 0, 0, 0, 
    0,    0, 0, 0, 0, 0, 0, 0, 
    0,    0, 0, 0, 0, 0, 0, 0, 
    0,    0, 0, 0, 0, 0, 0, 0, 
    0,    0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 
    0,    0, 0, 0, 0, 0, 0, 0, 
    0,    0, 0, 0, 0, 0, 0, 0,
};

static inline u32 to_be32(const u32 in)
{
    u32 ret;

#if (ENDIANESS == ORDER_LITTLE_ENDIAN)
    ret = ll_bswap4(in);
#else 
    ret = in;
#endif

    return ret;
}

static inline u64 to_be64(u64 in)
{
    u64 ret;

#if (ENDIANESS == ORDER_LITTLE_ENDIAN)
    ret = ll_bswap8(in);
#else 
    ret = in;
#endif

    return ret;
}

static inline void clean(u8 *p, size_t plen)
{
    size_t i;
    volatile u8 *pp;

    i = 0;
    pp = (volatile u8*)p;
    while (i < plen)
        pp[i++] = 0;
}

const char* sm3_get_impl_name()
{
    static const char *impl_name = "armv8 neon";
    return impl_name;
}

int sm3_init(SM3_CTX *ctx)
{
    ctx->digest[0] = 0x7380166FU;
    ctx->digest[1] = 0x4914B2B9U;
    ctx->digest[2] = 0x172442D7U;
    ctx->digest[3] = 0xDA8A0600U;
    ctx->digest[4] = 0xA96F30BCU;
    ctx->digest[5] = 0x163138AAU;
    ctx->digest[6] = 0xE38DEE4DU;
    ctx->digest[7] = 0xB0FB0E4EU;

    ctx->bits = 0;
    return SM3_OK;
}

int sm3_update(SM3_CTX *ctx, const u8* data, size_t datalen)
{
    size_t n, left;

    /* number of bytes in ctx->buf */
    n = (ctx->bits >> 3) & 0x3fU;

    ctx->bits += (datalen << 3);
    left = SM3_BLOCK_SIZE - n;
    if (datalen < left) {
        memcpy(ctx->buf + n, data, datalen);
        return SM3_OK;
    } else {
        /* concatenate ctx->buf and data to make up one block */
        memcpy(ctx->buf + n, data, left);
        sm3_compress_neon(ctx->digest, ctx->buf, 1);
        data += left;
        datalen -= left;
    }

    /* compress the remaining data */
    n = (datalen >> 6);
    sm3_compress_neon(ctx->digest, data, n);

    data += (SM3_BLOCK_SIZE*n);
    datalen &= 0x3fU;

    /* coppy the last few bytes to ctx->buf */
    memcpy(ctx->buf, data, datalen);
    return SM3_OK;
}

int sm3_final(u8 *digest, SM3_CTX *ctx)
{
    int i;
    size_t n;
    u8 tbuf[SM3_BLOCK_SIZE];
    u32 tdigest[8];
    u32 *pdigest;

    /* number of bytes in ctx->buf */
    n = (ctx->bits >> 3) & 0x3fU;
    
    /* copy ctx */
    memcpy(tbuf, ctx->buf, n);
    tdigest[0] = ctx->digest[0];
    tdigest[1] = ctx->digest[1];
    tdigest[2] = ctx->digest[2];
    tdigest[3] = ctx->digest[3];
    tdigest[4] = ctx->digest[4];
    tdigest[5] = ctx->digest[5];
    tdigest[6] = ctx->digest[6];
    tdigest[7] = ctx->digest[7];
    pdigest = (u32*)digest;

    if (n < 56)
        memcpy(tbuf + n, PAD, 56 - n);
    else {
        memcpy(tbuf + n, PAD, 64 - n);
        sm3_compress_neon(tdigest, tbuf, 1);
        memset(tbuf, 0, 56);
    }

    *(u64*)(&tbuf[56]) = to_be64(ctx->bits);
    sm3_compress_neon(tdigest, tbuf, 1);

    /* big endian */
    for (i = 0; i < 8; i++)
        pdigest[i] = to_be32(tdigest[i]);

    clean(tbuf, SM3_BLOCK_SIZE);
    return SM3_OK;
}

void sm3_clean(SM3_CTX *sm3_ctx)
{
    clean((u8*)sm3_ctx, sizeof(SM3_CTX));
}

int sm3_once(u8 digest[SM3_DIGEST_LENGTH], const u8 *msg, size_t msglen)
{
    SM3_CTX sm3_ctx;
    sm3_init(&sm3_ctx);
    sm3_update(&sm3_ctx, msg, msglen);
    sm3_final(digest, &sm3_ctx);
    sm3_clean(&sm3_ctx);
    return SM3_OK;
}
