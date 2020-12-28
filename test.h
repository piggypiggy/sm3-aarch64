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

#include <sm3.h>
#include <stdio.h>
#include <time.h>
#include "config.h"

int u8_to_hex(u8 *out, const u8 *in, size_t inlen);

int hex_to_u8(u8 *out, const u8 *in, size_t inlen);

void print_hex(const char *desp, const u8 *s, size_t slen);

void random_string(u8 *s, size_t len);

size_t random_number(void);
