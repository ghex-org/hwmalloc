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

#include <hwmalloc/config.hpp>

// use the variadic macro HWMALLOC_LOG to log messages
// the arguments will be divided by a whitespace
#ifdef HWMALLOC_ENABLE_LOGGING
#define HWMALLOC_LOG(...) ::hwmalloc::log_message(__VA_ARGS__, "(", __FILE__, ":", __LINE__, ")");
#else
#define HWMALLOC_LOG(...)
#endif

// implementation
#ifdef HWMALLOC_ENABLE_LOGGING
#include <sstream>

namespace hwmalloc
{
namespace detail
{
std::stringstream& log_stream();

void print_log_message(std::stringstream&);

void log_message(std::stringstream&);

template<typename S, typename... Rest>
void
log_message(std::stringstream& str, S&& s, Rest&&... r)
{
    str << " " << s;
    log_message(str, std::forward<Rest>(r)...);
}

} // namespace detail

// main logging function
template<typename... S>
void
log_message(S&&... s)
{
    auto& str = detail::log_stream();
    detail::log_message(str, std::forward<S>(s)...);
    detail::print_log_message(str);
}

} // namespace hwmalloc
#endif
