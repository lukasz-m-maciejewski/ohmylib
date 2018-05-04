#ifndef MY_MEMORY_HPP
#define MY_MEMORY_HPP

#include "type_traits.hpp"
#include "utility.hpp"

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
    template <typename PtrType, typename PtrDeleter, typename = void>
    struct PointerTypeDeductionHelper
    {
        using type = PtrType*;
    };

    template <typename PtrType, typename PtrDeleter>
    struct PointerTypeDeductionHelper<
        PtrType, PtrDeleter,
        void_t<typename remove_reference_t<PtrDeleter>::pointer>>
    {
        using type = typename remove_reference_t<PtrDeleter>::pointer;
    };

public:
    using DeleterConstraint = enable_if_t<not is_pointer_v<Deleter> and
                                          is_default_constructible_v<Deleter>>;

    using pointer = typename PointerTypeDeductionHelper<Type, Deleter>::type;

    unique_ptr_impl() = default;
    unique_ptr_impl(pointer ptr) : m_ptr{ptr}, m_deleter{}
    {
    }

    template <typename OtherDeleter>
    unique_ptr_impl(pointer ptr, OtherDeleter&& deleter)
        : m_ptr{ptr}, m_deleter{forward<OtherDeleter>(deleter)}
    {
    }

    pointer& get_ptr()
    {
        return m_ptr;
    }
    pointer get_ptr() const
    {
        return m_ptr;
    }
    Deleter& get_deleter()
    {
        return m_deleter;
    }
    const Deleter& get_deleter() const
    {
        return m_deleter;
    }

private:
    pointer m_ptr;
    Deleter m_deleter;
};
} // namespace detail

template <typename Type, typename Deleter = default_delete<Type>>
class unique_ptr
{
    template <typename U>
    using DeleterConstraint =
        typename detail::unique_ptr_impl<Type, U>::DeleterConstraint::type;

    detail::unique_ptr_impl<Type, Deleter> m_impl;

public:
    using pointer = typename detail::unique_ptr_impl<Type, Deleter>::pointer;
    using element_type = Type;
    using deleter_type = Deleter;

    template <typename U, typename E>
    using safe_conversion_up = detail::operator_and<
        is_convertible<typename unique_ptr<U, E>::pointer, pointer>,
        detail::operator_not<is_array<U>>,
        detail::operator_or<
            detail::operator_and<is_reference<deleter_type>,
                                 is_same<deleter_type, E>>,
            detail::operator_and<
                detail::operator_not<is_reference<deleter_type>>,
                is_convertible<E, deleter_type>>>>;
    template <typename U, typename E>
    inline constexpr static bool safe_conversion_v =
        is_convertible_v<typename unique_ptr<U, E>::pointer, pointer> and
        not is_array_v<U> and
        ((is_reference_v<deleter_type> and is_same_v<deleter_type, E>) or
         (not is_reference_v<deleter_type> and
          is_convertible_v<deleter_type, E>));

    template <typename U = Deleter, typename = DeleterConstraint<U>>
    constexpr unique_ptr() noexcept : m_impl{}
    {
    }

    template <typename U = Deleter, typename = DeleterConstraint<U>>
    explicit unique_ptr(pointer ptr) noexcept : m_impl{ptr}
    {
    }

    unique_ptr(pointer ptr, conditional_t<is_reference_v<deleter_type>,
                                          deleter_type, const deleter_type&>
                                deleter) noexcept
        : m_impl{ptr, deleter}
    {
    }

    unique_ptr(pointer ptr, remove_reference_t<deleter_type>&& deleter) noexcept
        : m_impl{move(ptr), move(deleter)}
    {

        static_assert(not is_reference_v<deleter_type>,
                      "rvalue deleter bound to reference");
    }
};

} // namespace ohmy
#endif // MY_MEMORY_HPP
