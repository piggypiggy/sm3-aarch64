/******************************************************************************
 *                                                                            *
 * Copyright 2020 Meng-Shan Jiang                                             *
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

#pragma once

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#define SM3_OK    0
#define SM3_ERR  -1

#define SM3_DIGEST_LENGTH   32
#define SM3_BLOCK_SIZE      64

#define u8  unsigned char
#define u32 unsigned int
#define u64 unsigned long long

#define ORDER_BIG_ENDIAN     0
#define ORDER_LITTLE_ENDIAN  1

#if defined(BUILD_STATIC)
# define PIGGY_EXPORT
#else
# if defined(_MSC_VER)
#  if defined(BUILD_SHARED)
#   define PIGGY_EXPORT __declspec(dllexport)
#  else
#   define PIGGY_EXPORT __declspec(dllimport)
#  endif
# elif (defined(__GNUC__) && __GNUC__>= 4)
#  define PIGGY_EXPORT __attribute__ ((visibility ("default")))
# else
#  define PIGGY_EXPORT
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    u8 buf[SM3_BLOCK_SIZE];
    u32 digest[8];
    size_t bits;
} SM3_CTX;

PIGGY_EXPORT const char* sm3_get_impl_name();

PIGGY_EXPORT int sm3_init(SM3_CTX *ctx);

PIGGY_EXPORT int sm3_update(SM3_CTX *ctx, const u8* data, size_t datalen);

PIGGY_EXPORT int sm3_final(u8 *digest, SM3_CTX *ctx);

PIGGY_EXPORT void sm3_clean(SM3_CTX *sm3_ctx);

PIGGY_EXPORT int sm3_once(u8 digest[SM3_DIGEST_LENGTH], const u8 *msg, size_t msglen);

#ifdef __cplusplus
}
#endif
