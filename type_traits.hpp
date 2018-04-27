#ifndef MY_TYPE_TRAITS_HPP
#define MY_TYPE_TRAITS_HPP

namespace ohmy
{
template <typename Type, Type parameter_value>
struct integral_constant
{
    static constexpr Type value = parameter_value;
    using value_type = Type;
    using type = integral_constant<Type, parameter_value>;

    constexpr operator value_type() const noexcept
    {
        return value;
    }

    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

// static member type initialization??
// template <typename Type, Type parameter_value>
// constexpr Type integral_constant<Type, parameter_value>::value;

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template <bool parameter_value>
using bool_constant = integral_constant<bool, parameter_value>;

template <typename, typename>
struct is_same : public false_type
{
};

template <typename Type>
struct is_same<Type, Type> : public true_type
{
};

template <typename Base, typename Derived>
struct is_base_of : public integral_constant<bool, __is_base_of(Base, Derived)>
{
};

template <bool Condition, typename IfTrue, typename IfFalse>
struct conditional
{
    using type = IfTrue;
};

template <typename IfTrue, typename IfFalse>
struct conditional<false, IfTrue, IfFalse>
{
    using type = IfFalse;
};

template <typename Type>
struct remove_const
{
    using type = Type;
};

template <typename Type>
struct remove_const<Type const>
{
    using type = Type;
};

template <typename Type>
struct remove_volatile
{
    using type = Type;
};

template <typename Type>
struct remove_volatile<Type volatile>
{
    using type = Type;
};

template <typename Type>
struct remove_cv
{
    using type =
        typename remove_const<typename remove_volatile<Type>::type>::type;
};

namespace detail
{
template <typename...>
struct operator_or;

template <>
struct operator_or<> : public false_type
{
};

template <typename Bool1>
struct operator_or<Bool1> : public Bool1
{
};

template <typename Bool1, typename Bool2>
struct operator_or<Bool1, Bool2>
    : public conditional<Bool1::value, Bool1, Bool2>::type
{
};

template <typename Bool1, typename Bool2, typename Bool3, typename... Bools>
struct operator_or<Bool1, Bool2, Bool3, Bools...>
    : public conditional<Bool1::value, Bool1,
                         operator_or<Bool2, Bool3, Bools...>>::type
{
};

template <typename>
struct is_void_helper : public false_type
{
};

template <>
struct is_void_helper<void> : public true_type
{
};

} // namespace detail

template <typename Type>
struct is_void
    : public detail::is_void_helper<typename remove_cv<Type>::type>::type
{
};

template <typename Type>
struct is_const : false_type
{
};

template <typename Type>
struct is_const<Type const> : true_type
{
};

template <typename Type>
struct is_lvalue_reference : false_type
{
};

template <typename Type>
struct is_lvalue_reference<Type&> : true_type
{
};

template <typename Type>
struct is_rvalue_reference : false_type
{
};

template <typename Type>
struct is_rvalue_reference<Type&&> : true_type
{
};

template <typename Type>
struct is_reference
    : detail::operator_or<is_lvalue_reference<Type>, is_rvalue_reference<Type>>
{
};

template <typename Type>
struct is_function
    : bool_constant<!is_const<Type const>::value && !is_reference<Type>::value>
{
};

template <typename Type>
struct is_array : public false_type
{
};

namespace detail
{
template <typename Type>
struct declval_protector
{
    static const bool stop = false;
    static Type delegate();
};
} // namespace detail

template <typename Type>
inline Type declval() noexcept
{
    static_assert(detail::declval_protector<Type>::stop,
                  "declval() should not be called in runtime context");
    return detail::declval_protector<Type>::delegate();
}

namespace detail
{
template <typename From, typename To,
          bool = detail::operator_or<is_void<From>, is_function<To>,
                                     is_array<To>>::value>
struct priv_is_convertible_helper
{
    using type = typename is_void<To>::type;
};

template <typename From, typename To>
struct priv_is_convertible_helper<From, To, false>
{
private:
    template <typename To_Internal>
    static void test_function_aux(To_Internal);

    template <typename From_Internal, typename To_Internal,
              typename = decltype(
                  test_function_aux<To_Internal>(declval<From_Internal>()))>
    static true_type test_function(int);

    template <typename, typename>
    static false_type test_function(...);

public:
    using type = decltype(test_function<From, To>(0));
};
} // namespace detail

} // namespace ohmy

#endif // MY_TYPE_TRAITS_HPP