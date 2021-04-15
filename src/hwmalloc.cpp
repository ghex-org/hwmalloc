#include <hwmalloc/hwmalloc.hpp>

namespace hwmalloc
{
namespace detail
{
class pool
{
  public:
    hwvptr make() const noexcept { return {}; }
};

pool p_;

} // namespace detail

hwvptr malloc(std::size_t) { return detail::p_.make(); }

void
free(hwvptr const&)
{
}

} // namespace hwmalloc
