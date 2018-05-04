#ifndef MY_TYPE_TRAITS_HPP
#define MY_TYPE_TRAITS_HPP

namespace ohmy
{
using size_t = unsigned long long int;

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

template <typename Type1, typename Type2>
inline constexpr bool is_same_v = is_same<Type1, Type2>::value;

template <typename Base, typename Derived>
struct is_base_of : public integral_constant<bool, __is_base_of(Base, Derived)>
{
};

template <typename Base, typename Derived>
inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

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

template <bool Condition, typename IfTrue, typename IfFalse>
using conditional_t = typename conditional<Condition, IfTrue, IfFalse>::type;

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
using remove_const_t = typename remove_const<Type>::type;

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
using remove_volatile_t = typename remove_volatile<Type>::type;

template <typename Type>
struct remove_cv
{
    using type =
        typename remove_const<typename remove_volatile<Type>::type>::type;
};

template <typename Type>
using remove_cv_t = typename remove_cv<Type>::type;

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

template <typename Bool>
struct operator_not : bool_constant<!Bool::value>
{
};

template <typename...>
struct operator_and;

template <>
struct operator_and<> : true_type
{
};

template <typename Bool1>
struct operator_and<Bool1> : Bool1
{
};

template <typename Bool1, typename Bool2>
struct operator_and<Bool1, Bool2> : conditional_t<Bool1::value, Bool2, Bool1>
{
};

template <typename Bool1, typename Bool2, typename Bool3, typename... Bools>
struct operator_and<Bool1, Bool2, Bool3, Bools...>
    : conditional_t<Bool1::value, operator_and<Bool2, Bool3, Bools...>, Bool1>
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
inline constexpr bool is_void_v = is_void<Type>::value;

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
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<Type>::value;

template <typename Type>
struct is_rvalue_reference : false_type
{
};

template <typename Type>
struct is_rvalue_reference<Type&&> : true_type
{
};

template <typename Type>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<Type>::value;

template <typename Type>
struct is_reference
    : detail::operator_or<is_lvalue_reference<Type>, is_rvalue_reference<Type>>
{
};

template <typename Type>
inline constexpr bool is_reference_v = is_reference<Type>::value;

template <typename Type>
struct is_function
    : bool_constant<!is_const<Type const>::value && !is_reference<Type>::value>
{
};

template <typename Type>
inline constexpr bool is_function_v = is_function<Type>::value;

template <typename>
struct is_array : false_type
{
};

template <typename Type, size_t Size>
struct is_array<Type[Size]> : true_type
{
};

template <typename Type>
struct is_array<Type[]> : true_type
{
};

template <typename Type>
inline constexpr bool is_array_v = is_array<Type>::value;

namespace detail
{
template <typename Type>
struct is_pointer_helper : false_type
{
};

template <typename Type>
struct is_pointer_helper<Type*> : true_type
{
};
} // namespace detail

template <typename Type>
struct is_pointer : detail::is_pointer_helper<remove_cv_t<Type>>::type
{
};

template <typename Type>
inline constexpr bool is_pointer_v = is_pointer<Type>::value;

template <typename Type>
struct is_object
    : detail::operator_not<detail::operator_or<
          is_function<Type>, is_reference<Type>, is_void<Type>>>::type
{
};

template <typename Type>
inline constexpr bool is_object_v = is_object<Type>::value;

namespace detail
{
template <typename Type>
struct is_referencable : operator_or<is_object<Type>, is_reference<Type>>::type
{
};

template <typename Type>
inline constexpr bool is_referencable_v = is_referencable<Type>::value;

template <typename Type, bool = is_referencable_v<Type>>
struct add_lvalue_reference_helper
{
    using type = Type;
};

template <typename Type>
struct add_lvalue_reference_helper<Type, true>
{
    using type = Type&;
};
} // namespace detail

template <typename Type>
struct add_lvalue_reference : detail::add_lvalue_reference_helper<Type>
{
};

template <typename Type>
using add_lvalue_reference_t = typename add_lvalue_reference<Type>::type;

namespace detail
{
template <typename Type, bool = is_referencable<Type>::value>
struct add_rvalue_reference_helper
{
    using type = Type;
};

template <typename Type>
struct add_rvalue_reference_helper<Type, true>
{
    using type = Type&&;
};
} // namespace detail

template <typename Type>
struct add_rvalue_reference : detail::add_rvalue_reference_helper<Type>
{
};

template <typename Type>
using add_rvalue_reference_t = typename add_rvalue_reference<Type>::type;

template <typename Type>
struct remove_reference
{
    using type = Type;
};

template <typename Type>
struct remove_reference<Type&>
{
    using type = Type;
};

template <typename Type>
struct remove_reference<Type&&>
{
    using type = Type;
};

template <typename Type>
using remove_reference_t = typename remove_reference<Type>::type;

namespace detail
{
template <typename Type>
struct declval_protector
{
    static const bool stop = false;
    static add_rvalue_reference_t<Type> delegate();
};
} // namespace detail

template <typename Type>
inline add_rvalue_reference_t<Type> declval() noexcept
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
struct is_convertible_helper
{
    using type = typename is_void<To>::type;
};

template <typename From, typename To>
struct is_convertible_helper<From, To, false>
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

template <typename From, typename To>
struct is_convertible : detail::is_convertible_helper<From, To>::type
{
};

template <typename From, typename To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

template <bool, typename Type = void>
struct enable_if
{
};

template <typename Type>
struct enable_if<true, Type>
{
    using type = Type;
};

template <bool Condition, typename Type = void>
using enable_if_t = typename enable_if<Condition, Type>::type;

template <typename...>
using void_t = void;

template <typename>
struct rank : integral_constant<size_t, size_t{}>
{
};

template <typename Type, size_t Size>
struct rank<Type[Size]> : integral_constant<size_t, 1 + rank<Type>::value>
{
};

template <typename Type>
struct rank<Type[]> : integral_constant<size_t, 1 + rank<Type>::value>
{
};

template <typename, unsigned>
struct extent : integral_constant<size_t, 0ull>
{
};

template <typename Type, unsigned Uint, size_t Size>
struct extent<Type[Size], Uint>
    : integral_constant<size_t,
                        Uint == 0 ? Size : extent<Type, Uint - 1>::value>
{
};

template <typename Type, unsigned Uint>
struct extent<Type[], Uint>
    : integral_constant<size_t, Uint == 0 ? 0 : extent<Type, Uint - 1>::value>
{
};

template <typename Type, unsigned Index = 0>
inline constexpr size_t extent_v = extent<Type, Index>::value;

template <typename Type>
struct remove_extent
{
    using type = Type;
};

template <typename Type, size_t Size>
struct remove_extent<Type[Size]>
{
    using type = Type;
};

template <typename Type>
struct remove_extent<Type[]>
{
    using type = Type;
};

template <typename Type>
using remove_extent_t = typename remove_extent<Type>::type;

template <typename Type>
struct remove_all_extents
{
    using type = Type;
};

template <typename Type, size_t Size>
struct remove_all_extents<Type[Size]>
{
    using type = typename remove_all_extents<Type>::type;
};

template <typename Type>
struct remove_all_extents<Type[]>
{
    using type = typename remove_all_extents<Type>::type;
};

template <typename Type>
using remove_all_extents_t = typename remove_all_extents<Type>::type;

namespace detail
{
template <typename Type>
struct is_array_known_bounds : bool_constant<(extent_v<Type>> 0)>
{
};

struct do_is_default_constructible_impl
{
    template <typename Type, typename = decltype(Type())>
    static true_type test_function(int);

    template <typename>
    static false_type test_function(...);
};

template <typename Type>
struct is_default_constructible_impl : do_is_default_constructible_impl
{
    using type = decltype(test_function<Type>(0));
};

template <typename Type>
struct is_default_constructible_atom
    : operator_and<operator_not<is_void<Type>>,
                   is_default_constructible_impl<Type>>
{
};

template <typename Type, bool = is_array_v<Type>>
struct is_default_constructible_safe;

template <typename Type>
struct is_default_constructible_safe<Type, true>
    : operator_and<is_array_known_bounds<Type>,
                   is_default_constructible_atom<remove_all_extents_t<Type>>>
{
};

template <typename Type>
struct is_default_constructible_safe<Type, false>
    : is_default_constructible_atom<Type>::type
{
};

} // namespace detail

template <typename Type>
struct is_default_constructible
    : detail::is_default_constructible_safe<Type>::type
{
};

template <typename Type>
inline constexpr bool is_default_constructible_v =
    is_default_constructible<Type>::value;

namespace detail
{
template <typename From, typename To>
inline constexpr bool = (not (is_void_v<From> or is_function_v<From>)) ? ;

struct do_is_nary_constructible_impl
{
    template <typename Type, typename... Args,
              typename = decltype(Type(declval<Args>()...))>
    static true_type test_function(int);

    template <typename, typename...>
    static false_type test_function(...);
};

template <typename Type, typename... Args>
struct is_nary_constructible_impl : do_is_nary_constructible_impl
{
    using type = decltype(test_function<Type, Args...>(0));
};

template <typename Type, typename... Args>
struct is_nary_constructible : is_nary_constructible_impl<Type, Args...>::type
{
    static_assert(sizeof...(Args) > 1);
};
} // namespace detail

template <typename Type, typename... Arguments>
struct is_nothrow_constructible
    : operator_and<is_constructible<Type, Args...>,
                   detail::is_nothrow_constructible_impl<Type, Args...>>
{
};

namespace detail
{
template <typename Type, bool = is_referencable_v<Type>>
struct is_nothrow_move_constructible_impl;

template <typename Type>
struct is_nothrow_move_constructible_impl<Type, false> : false_type
{
};

template <typename Type>
struct is_nothrow_move_constructible_impl<Type, true>
    : is_nothrow_constructible<Type, Type&&>
{
};
} // namespace detail

} // namespace ohmy

#endif // MY_TYPE_TRAITS_HPP
