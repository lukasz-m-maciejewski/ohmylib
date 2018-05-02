#ifndef MY_MEMORY_HPP
#define MY_MEMORY_HPP

#include "type_traits.hpp"

namespace ohmy
{
template <typename Type>
struct default_delete
{
    constexpr default_delete() noexcept = default;

    template <typename Other,
              typename = enable_if_t<is_convertible_v<Other, Type>>>
    default_delete(const default_delete<Other>&) noexcept
    {
    }

    void operator()(Type* ptr) const
    {
        static_assert(!is_void_v<Type>,
                      "cannot delete pointer to incomplete type");
        static_assert(sizeof(Type) > 0,
                      "cannot delete pointer to incomplete type");
        delete ptr;
    }
};

template <typename Type>
struct default_delete<Type[]>
{
    constexpr default_delete() noexcept = default;

    template <typename Other,
              typename = enable_if_t<is_convertible_v<Other (*)[], Type (*)[]>>>
    default_delete(const default_delete<Other[]>&) noexcept
    {
    }

    template <typename Other>
    enable_if_t<is_convertible_v<Other (*)[], Type (*)[]>>
    operator()(Other* ptr) const
    {
        static_assert(sizeof(Type) > 0,
                      "cannot delete pointer to incomplete type");
        delete[] ptr;
    }
};

namespace detail
{
template <typename Type, typename Deleter>
class unique_ptr_impl
{
    template <typename U, typename E, typename = void>
    struct Pointer
    {
        using type = U*;
    };

    template <typename U, typename E>
    struct Pointer<U, E, void_t<typename remove_reference_t<E>::pointer>>
    {
        using type = typename remove_reference_t<E>::pointer;
    };

public:
    using DeleterConstraint = enable_if_t<not is_pointer_v<Deleter> and is_default_constructible_v<Deleter>>;
};
} // namespace detail
} // namespace ohmy
#endif // MY_MEMORY_HPP
