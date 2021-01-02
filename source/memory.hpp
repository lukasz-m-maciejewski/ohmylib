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

    template <typename U = Deleter, typename = DeleterConstraint<U>>
    constexpr unique_ptr(nullptr_t) noexcept : unique_ptr()
    {
    }

    unique_ptr(unique_ptr&& up) noexcept
        : m_impl{up.release(), forward<deleter_type>(up.get_deleter())}
    {
    }

    template <
        typename U, typename E,
        typename = detail::require_t<
            safe_conversion_up<U, E>,
            typename conditional<is_reference_v<Deleter>, is_same<E, Deleter>,
                                 is_convertible<E, Deleter>>::type>>
    unique_ptr(unique_ptr<U, E>&& up) noexcept
        : m_impl{up.release(), forward<E>(up.get_deleter())}
    {
    }

    ~unique_ptr() noexcept
    {
        auto& ptr = m_impl.get_ptr();
        if (ptr != nullptr)
        {
            get_deleter()(ptr);
            ptr = pointer{};
        }
    }

    unique_ptr& operator=(unique_ptr&& up) noexcept
    {
        reset(up.release());
        get_deleter() = forward<deleter_type>(up.get_deleter());
        return *this;
    }

    template <typename U, typename E>
    enable_if_t<safe_conversion_v<U, E> and is_assignable_v<deleter_type&, E&&>,
                unique_ptr&>
    operator=(unique_ptr<U, E> up) noexcept
    {
        reset(up.release());
        get_deleter() = forward<E>(up.get_deleter());
        return *this;
    }

    unique_ptr& operator=(nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    add_lvalue_reference_t<element_type> operator*() const
    {
        *get();
    }

    pointer operator->() const noexcept
    {
        return get();
    }

    pointer get() const noexcept
    {
        return m_impl.get_ptr();
    }

    deleter_type& get_deleter() noexcept
    {
        return m_impl.get_deleter();
    }

    const deleter_type& get_deleter() const noexcept
    {
        return m_impl.get_deleter();
    }

    explicit operator bool() const noexcept
    {
        return (get() != pointer{});
    }

    pointer release() noexcept
    {
        pointer p = get();
        m_impl.m_ptr() = pointer{};
        return p;
    }

    void reset(pointer p = pointer{}) noexcept
    {
        using ohmy::swap;
        swap(m_impl.get_ptr(), p);
        if (p != pointer{})
        {
            get_deleter()(p);
        }
    }

    void swap(unique_ptr& up) noexcept
    {
        using ohmy::swap;
        swap(m_impl, up.m_impl);
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
};

template <typename Type>
inline constexpr Type* addressof(Type& r) noexcept
{
    return __builtin_addressof(r);
}

template <typename Type>
const Type* addressof(const Type&&) = delete;

} // namespace ohmy
#endif // MY_MEMORY_HPP
