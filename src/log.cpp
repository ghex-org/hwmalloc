/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <hwmalloc/log.hpp>
#include <iostream>
#include <mutex>

namespace hwmalloc
{
namespace detail
{
namespace
{
std::mutex&
log_mutex()
{
    static std::mutex m;
    return m;
}
} // namespace

std::stringstream&
log_stream()
{
    constexpr char                        prefix[] = "HWMALLOC:";
    static thread_local std::stringstream str;
    str.clear();
    str << prefix;
    return str;
}

void
print_log_message(std::stringstream& str)
{
    std::lock_guard<std::mutex> m(log_mutex());
    str << "\n";
    str >> std::cerr.rdbuf();
    std::cerr.flush();
}

void
log_message(std::stringstream&)
{
}

} // namespace detail
} // namespace hwmalloc
