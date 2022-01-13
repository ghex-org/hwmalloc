/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <array>
#include <memory>

namespace hwmalloc
{

// this only exists so that a nice error message with the sizes
// is printed when the assertion fails
template<int B, int S>
struct CheckSizes
{
  static_assert(B>=S, "Reserved space is too small");
  static constexpr bool value = (B>=S);
};

struct handle_base
{
    virtual uint64_t       get_remote_key(void) const = 0;
    virtual void          *get_local_key(void) const = 0;
    virtual unsigned char *get_address(void) const = 0;
    virtual uint32_t get_message_length(void) const { return 0; }
    virtual void     set_message_length(uint32_t) { }
};

// This base class is required for type erased access to pointers
// from code that does not know the 'type' of the heap used
struct allocator_base
{
    using size_type = std::size_t;

    // This must always be large enough to hold a block
    // that is stored in the smart pointeer returned by the
    // true allocator. use 64 for now as it is usually one cache line
    static constexpr uint32_t const block_space = 64 - 2*sizeof(void*);


    // this is just an alias for the contents of the allocation fancy pointer
    // structure that will be cast to anonymous data
    struct anonymous_handle {
        // these are from segment.hpp
        //        segment*    m_segment = nullptr;
        //        void*       m_ptr = nullptr
        void *segment_ptr_;
        void *addr_;
        std::unique_ptr<handle_base> provider_handle_;

        // Make this move only as we have a unique_ptr embedded
        ~anonymous_handle() = default;
        anonymous_handle() = default;
        anonymous_handle(anonymous_handle const& other) = delete;
        anonymous_handle(anonymous_handle && other) = default;
        anonymous_handle& operator=(anonymous_handle const& other) = delete;
        anonymous_handle& operator=(anonymous_handle && other) = default;
    };

    virtual anonymous_handle allocate_untyped(size_type) = 0;
    virtual void deallocate_untyped(anonymous_handle &data) = 0;
};

} // namespace hwmalloc
