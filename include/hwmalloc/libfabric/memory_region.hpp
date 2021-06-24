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

#include <hwmalloc/register.hpp>
//
#include <rdma/fabric.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_eq.h>
#include <rdma/fi_errno.h>
//
#include <iostream>
#include <memory>
#include <utility>
//

namespace hwmalloc
{
namespace libfabric
{
// This is the ony part of the code that actually
// calls libfabric functions
struct region_provider
{
    // The internal memory region handle
    typedef struct fid_mr     provider_region;
    typedef struct fid_domain provider_domain;

    // register region
    template<typename... Args>
    static int register_memory(Args&&... args)
    {
        return fi_mr_reg(std::forward<Args>(args)...);
    }

    // unregister region
    static int unregister_memory(provider_region* region) { return fi_close(&region->fid); }

    // Default registration flags for this provider
    static int flags()
    {
        return FI_READ | FI_WRITE | FI_RECV | FI_SEND | FI_REMOTE_READ | FI_REMOTE_WRITE;
    }

    // Get the local descriptor of the memory region.
    static void* get_local_key(provider_region* region) { return fi_mr_desc(region); }

    // Get the remote key of the memory region.
    static uint64_t get_remote_key(provider_region* region) { return fi_mr_key(region); }
};

// --------------------------------------------------------------------
// This is a handle to a small chunk of memory that has been registered
// as part of a much larger allocation (a segment)
struct memory_handle
{
    using provider_domain = region_provider::provider_domain;
    using provider_region = region_provider::provider_region;

    // Default constructor creates unusable handle(region)
    memory_handle() {}
    memory_handle(memory_handle const&) noexcept = default;
    memory_handle& operator=(memory_handle const&) noexcept = default;
    memory_handle(memory_handle&&) noexcept = default;

    memory_handle(provider_region* region, char* addr, std::size_t size)
    : local_key_{region_provider::get_local_key(region)}
    , remote_key_{region_provider::get_remote_key(region)}
    , address_{addr}
    , size_{size}
    , used_space_{0}
    {
    }

    // --------------------------------------------------------------------
    // Get the local descriptor of the memory region.
    inline void* get_local_key(void) const { return local_key_; }

    // --------------------------------------------------------------------
    // Get the remote key of the memory region.
    inline uint64_t get_remote_key(void) const { return remote_key_; }

    // --------------------------------------------------------------------
    // return the address of this memory region block. If this
    // is a partial region, then the address will be offset from the
    // base address
    inline char* get_address(void) const { return address_; }

    // --------------------------------------------------------------------
    // Get the size of the memory chunk usable by this memory region,
    // this may be smaller than the value returned by get_length
    // if the region is a sub region (partial region) within another block
    inline uint64_t get_size(void) const { return size_; }

    // --------------------------------------------------------------------
    // Get the size used by a message in the memory region.
    inline uint32_t get_message_length(void) const { return used_space_; }

    // --------------------------------------------------------------------
    // Set the size used by a message in the memory region.
    inline void set_message_length(uint32_t length) { used_space_ = length; }

  protected:
    // we may be a piece of a larger region, this gives the start address
    // of this piece of the region. This is the address that should be used for data
    // storage
    char* address_;

    // local/remote keys cached from fi_mr_desc
    void*    local_key_;
    uint64_t remote_key_;

    // The size of the memory buffer, if this is a partial region
    // it will be smaller than the value returned by region_->length
    uint64_t size_;

    // space used by a message in the memory region.
    uint64_t used_space_;
};

// --------------------------------------------------------------------
// a memory segment is a pinned block of memory that has been specialized
// by a particular region provider. Each provider (infiniband, libfabric,
// other) has a different definition for the object and the protection
// domain used to limit access.
// --------------------------------------------------------------------
struct memory_segment : public memory_handle
{
    using provider_domain = region_provider::provider_domain;
    using provider_region = region_provider::provider_region;
    using handle_type = memory_handle;

    // --------------------------------------------------------------------
    // flags used for management of lifetime
    enum
    {
        BLOCK_USER = 1,
        BLOCK_TEMP = 2,
        BLOCK_PARTIAL = 4,
    };

    //        memory_segment()
    //            : memory_handle()
    //            , region_(nullptr)
    //            , base_addr_(nullptr)
    //            , flags_(0)
    //        {}

    memory_segment(
        provider_region* region, char* address, char* base_address, uint64_t size, uint32_t flags)
    : memory_handle(region, address, size)
    , region_(region)
    , base_addr_(base_address)
    , flags_(flags)
    {
    }

    // --------------------------------------------------------------------
    // move constructor, clear other region
    memory_segment(memory_segment&& other) noexcept
    : memory_handle(std::move(other))
    , region_{std::exchange(other.region_, nullptr)}
    , base_addr_{std::exchange(other.base_addr_, nullptr)}
    , flags_{other.flags_}
    {
    }

    // --------------------------------------------------------------------
    // move assignment, clear other region
    memory_segment& operator=(memory_segment&& other) noexcept
    {
        memory_handle(std::move(other));
        region_ = std::exchange(other.region_, nullptr);
        base_addr_ = std::exchange(other.base_addr_, nullptr);
        flags_ = other.flags_;
    }

    // --------------------------------------------------------------------
    // construct a memory region object by registering an existing address buffer
    memory_segment(provider_domain* pd, const void* buffer, const uint64_t length)
    {
        memory_handle::address_ = static_cast<char*>(const_cast<void*>(buffer));
        memory_handle::size_ = length;
        memory_handle::used_space_ = length;
        //
        region_ = nullptr;
        base_addr_ = address_;
        flags_ = BLOCK_USER;

        int ret = region_provider::register_memory(pd, const_cast<void*>(buffer), length,
            region_provider::flags(), 0, (uint64_t)address_, 0, &(region_), nullptr);

        if (ret)
        {
            //                memr_deb.debug("error registering region ",
            //                    alloctools::debug::ptr(buffer), alloctools::debug::hex<6>(length));
        }
        else
        {
            //                memr_deb.trace("OK registering region ",
            //                    alloctools::debug::ptr(buffer), alloctools::debug::ptr(address_), "desc ",
            //                    alloctools::debug::ptr(fi_mr_desc(region_)), "rkey ",
            //                    alloctools::debug::ptr(fi_mr_key(region_)), "length ",
            //                    alloctools::debug::hex<6>(size_));
        }
    }

    // --------------------------------------------------------------------
    // destroy the region and memory according to flag settings
    ~memory_segment()
    {
        //            if (get_partial_region())
        //                return;
        release();
    }

    // --------------------------------------------------------------------
    // Deregister the memory region.
    // returns 0 when successful, -1 otherwise
    int release(void)
    {
        if (region_ != nullptr)
        {
            //                memr_deb.trace("About to release memory region with local key ",
            //                    alloctools::debug::ptr(get_local_key()));
            // get these before deleting/unregistering (for logging)
            const void* buffer = get_base_address();
            //                auto length = memr_deb.declare_variable<uint64_t>(get_size());
            //                (void) length;
            //
            if (region_provider::unregister_memory(region_))
            {
                //                    memr_deb.debug("Error, fi_close mr failed\n");
                return -1;
            }
            else
            {
                //                    memr_deb.trace("deregistered memory region with local key ",
                //                        alloctools::debug::ptr(get_local_key()), "at address ",
                //                        alloctools::debug::ptr(buffer), "with length ",
                //                        alloctools::debug::hex<6>(length));
            }
            if (!get_user_region())
            {
                //                    delete[](static_cast<const char*>(buffer));
            }
            region_ = nullptr;
        }
        return 0;
    }

    handle_type get_handle(std::size_t offset, std::size_t size) const noexcept
    {
        return memory_handle(region_, base_addr_ + offset, size);
    }

    // --------------------------------------------------------------------
    // return the underlying libfabric region handle
    inline provider_region* get_region() const { return region_; }

    // --------------------------------------------------------------------
    // Get the address of the base memory region.
    // This is the address of the memory allocated from the system
    inline char* get_base_address(void) const { return base_addr_; }

    // --------------------------------------------------------------------
    // A user allocated region uses memory allocted by the user.
    // on destruction, the memory is unregistered, but not deleted
    inline void set_user_region() { flags_ |= BLOCK_USER; }

    inline bool get_user_region() const { return (flags_ & BLOCK_USER) == BLOCK_USER; }

    // --------------------------------------------------------------------
    // A temp region is one that the memory pool is not managing
    // so it is unregistered and deleted when returned to the pool and not reused
    inline void set_temp_region() { flags_ |= BLOCK_TEMP; }

    inline bool get_temp_region() const { return (flags_ & BLOCK_TEMP) == BLOCK_TEMP; }

    // --------------------------------------------------------------------
    // a partial region is a subregion of a larger memory region
    // on destruction, it is not unregistered or deleted as the 'parent' region
    // will delete many partial regions on destruction
    inline void set_partial_region() { flags_ |= BLOCK_PARTIAL; }

    inline bool get_partial_region() const { return (flags_ & BLOCK_PARTIAL) == BLOCK_PARTIAL; }

    // --------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, memory_segment const& region)
    {
        //            os << "region " << alloctools::debug::ptr(&region) << " base address "
        //               << alloctools::debug::ptr(region.base_addr_) << " address "
        //               << alloctools::debug::ptr(region.address_) << " flags "
        //               << alloctools::debug::hex<2>(region.flags_) << " size "
        //               << alloctools::debug::hex<6>(region.size_) << " used_space "
        //               << alloctools::debug::hex<6>(region.used_space_) << " local key "
        //               << alloctools::debug::ptr(region.get_local_key()) << " remote key "
        //               << alloctools::debug::ptr(region.get_remote_key());
        return os;
    }

    //    private:

  public:
    // The hardware level handle to the region (as returned from libfabric fi_mr_reg)
    provider_region* region_;

    // if we are part of a larger region, this is the base address of
    // that larger region
    char* base_addr_;

    // flags to control lifetime of blocks
    uint32_t flags_;
};

// --------------------------------------------------------------------
struct context
{
  private:
    region_provider::provider_domain* pd_;

  public:
    context()
    : pd_(nullptr)
    {
    }

    context(region_provider::provider_domain* pd)
    : pd_(pd)
    {
        std::cout << "context constructor" << std::endl;
    }

    ~context() { std::cout << "context destructor" << std::endl; }

    region_provider::provider_domain* get_domain() { return pd_; }
};
} // namespace libfabric
} // namespace hwmalloc

// --------------------------------------------------------------------
namespace hwmalloc
{
auto
register_memory(hwmalloc::libfabric::context& c, void* ptr, std::size_t size)
{
    return hwmalloc::libfabric::memory_segment(c.get_domain(), ptr, size);
}
} // namespace hwmalloc
