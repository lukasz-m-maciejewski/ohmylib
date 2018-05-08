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

template <typename Type>
auto move_if_noexcept(Type& x) noexcept
    -> conditional_t<detail::move_if_noexcept_condition_v<Type>, const Type&,
                     Type&&>
{
    return move(x);
}

// clang-format off
template <typename Type>
inline enable_if_t<(not detail::is_tuple_like_v<Type>)
                   and is_move_constructible_v<Type>
                   and is_move_assignable_v<Type>>
swap(Type& a, Type& b) noexcept(is_nothrow_move_constructible_v<Type>
                                and is_nothrow_move_assignable_v<Type>)
{
    Type tmp = move(a);
    a = move(b);
    b = move(tmp);
}
// clang-format on
} // namespace ohmy

#endif // OHMY_UTILITY_HPP
