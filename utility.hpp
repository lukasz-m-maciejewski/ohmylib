#ifndef OHMY_UTILITY_HPP
#define OHMY_UTILITY_HPP

#include "type_traits.hpp"

namespace ohmy
{
template <typename Type>
constexpr Type&& forward(remove_reference_t<Type>& value) noexcept
{
    return static_cast<Type&&>(value);
}

template <typename Type>
constexpr Type&& forward(remove_reference_t<Type>&& value) noexcept
{
    static_assert(!is_lvalue_reference_v<Type>);
    return static_cast<Type&&>(value);
}

template <typename Type>
constexpr remove_reference_t<Type>&& move(Type&& value) noexcept
{
    return static_cast<remove_reference_t<Type>&&>(value);
}

namespace detail
{
template <typename Type>
inline constexpr bool move_if_noexcept_condition_v =
    (not is_nothrow_move_constructible_v<Type> and
     is_copy_constructible_v<Type>);
}
} // namespace ohmy

#endif // OHMY_UTILITY_HPP
