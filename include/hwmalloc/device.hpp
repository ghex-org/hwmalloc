/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <cstddef>

namespace hwmalloc
{
int get_num_devices();

int get_device_id();

void set_device_id(int id);

void* device_malloc(std::size_t size);

void device_free(void* ptr) noexcept;

void memcpy_to_device(void* dst, void const* src, std::size_t count);

void memcpy_to_host(void* dst, void const* src, std::size_t count);

} // namespace hwmalloc
