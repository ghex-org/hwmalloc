#include <hwmalloc/hwmalloc.hpp>

namespace hwmalloc
{
namespace detail
{
class pool
{
  public:
    hwvptr make() const noexcept { 
        hwvptr x;
        x.m_data = raw_malloc<memory_type::cpu>(1,0);
        return x; }
};

pool p_;

} // namespace detail

hwvptr malloc(std::size_t) { return detail::p_.make(); }

void
free(hwvptr const&)
{
}

} // namespace hwmalloc
