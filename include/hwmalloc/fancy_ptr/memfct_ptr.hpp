#pragma once

#include <utility>

namespace hwmalloc
{
template<typename MemberFunctionPtr>
struct pmfc;

template<typename R, typename T, typename... Args>
struct pmfc<R (T::*)(Args...)>
{
    typedef R (T::*pfm)(Args...);
    T*  m_ptr;
    pfm m_pmf;
    template<typename... Wargs>
    R operator()(Wargs&&... args) const
    {
        return (m_ptr->*m_pmf)(std::forward<Wargs>(args)...);
    }
};

template<typename R, typename T, typename... Args>
struct pmfc<R (T::*)(Args...) const>
{
    typedef R (T::*pfm)(Args...) const;
    T const* m_ptr;
    pfm      m_pmf;
    template<typename... Wargs>
    R operator()(Wargs&&... args) const
    {
        return (m_ptr->*m_pmf)(std::forward<Wargs>(args)...);
    }
};

} // namespace hwmalloc
