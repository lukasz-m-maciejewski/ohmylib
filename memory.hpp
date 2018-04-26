#ifndef MY_MEMORY_HPP
#define MY_MEMORY_HPP

#include "type_traits.hpp"

namespace ohmy
{
template <typename T>
struct default_delete
{
constexpr default_delete() noexcept = default;

template <typename U, 
typename = typename std::enable_if<std::is_convertible<U, T>::value>::type>
default_delete(const default_delete<U>&) noexcept
{
}

};
}
#endif // MY_MEMORY_HPP