#ifndef MY_FUNCTIONAL_HPP
#define MY_FUNCTIONAL_HPP

#include "c++config.hpp"
#include "memory.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

#ifdef __cpp_rtti
#include <typeinfo>
#endif

namespace ohmy
{

template <typename Type>
inline constexpr bool is_location_invariant_v = is_trivially_copiable_v<Type>;

namespace detail
{

class undefined_class;

union nocopy_types {
    void* object;
    const void* const_object;
    void (*function_pointer)();
    void (undefined_class::*member_pointer)();
};

union [[gnu::may_alias]] any_data
{
    void* access()
    {
        return &pod_data[0];
    }
    const void* access() const
    {
        return &pod_data[0];
    }

    template <typename Type>
    Type& access()
    {
        return *static_cast<Type*>(access());
    }

    template <typename Type>
    const Type& access() const
    {
        return *static_cast<const Type*>(access());
    }

    nocopy_types unused;
    char pod_data[sizeof(nocopy_types)];
};

enum class manager_operation
{
    get_type_info,
    get_function_ptr,
    clone_functor,
    destroy_functor
};

template <typename Type>
struct simple_type_wrapper
{
    simple_type_wrapper(Type value) : value(value)
    {
    }

    Type value;
};

template <typename Signature>
class function;

class function_base
{
public:
    static const ohmy::size_t max_size = sizeof(nocopy_types);
    static const ohmy::size_t max_align = __alignof__(nocopy_types);

    template <typename Functor>
    class base_manager
    {
    protected:
        static const bool stored_locally = is_location_invariant_v<Functor> and
                                           sizeof(Functor) <= max_size and
                                           __alignof__(Functor) <= max_align and
                                           max_align % __alignof(Functor) == 0;
        using local_storage = integral_constant<bool, stored_locally>;

        static Functor* get_pointer(const any_data& source)
        {
            const Functor* ptr =
                stored_locally ? addressof(source) : source.access<Functor*>();
            return const_cast<Functor*>(ptr);
        }

        // Clone a location-invariant function object that fits within
        // an any_data structure.
        static void clone(any_data& dest, const any_data& source, true_type)
        {
            ::new (dest.access()) Functor(source.access<Functor>());
        }

        // Clone a function object that is not location-invariant or that cannot
        // fit into an any_data structure.
        static void clone(any_data& dest, const any_data& source, false_type)
        {
            dest.access<Functor*>() = new Functor(*source.access<Functor*>());
        }

        // Destroying a location-invariant object may still require destruction.
        static void destroy(any_data& victim, true_type)
        {
            victim.access<Functor>().~Functor();
        }

        static void destroy(any_data& victim, false_type)
        {
            delete victim.access<Functor*>();
        }

    public:
        static bool manager(any_data& dest, const any_data& source,
                            manager_operation op)
        {
            switch (op)
            {
#if __cpp_rtti
            case manager_operation::get_type_info:
                dest.access<const std::type_info*>() = &typeid(Functor);
                break;
#endif
            case manager_operation::get_function_ptr:
                dest.access<Functor*>() = get_pointer(source);
                break;

            case manager_operation::clone_functor:
                clone(dest, source, local_storage());
                break;

            case manager_operation::destroy_functor:
                destroy(dest, local_storage());
                break;
            }
            return false;
        }

        static void init_functor(any_data& functor, Functor&& f)
        {
            if constexpr (stored_locally)
            {
                ::new (functor.access()) Functor(ohmy::move(f));
            }
            else
            {
                functor.access<Functor*>() = new Functor(ohmy::move(f));
            }
        }

        template <typename Signature>
        static bool non_empty_function(const function<Signature>& f)
        {
            return static_cast<bool>(f);
        }

        template <typename Type>
        static bool non_empty_function(Type* fp)
        {
            return fp != nullptr;
        }

        template <typename Class, typename Type>
        static bool non_empty_function(Type Class::*mp)
        {
            return mp != nullptr;
        }

        template <typename Type>
        static bool non_empty_function(const Type&)
        {
            return true;
        }
    };

    function_base() : manager(nullptr)
    {
    }

    ~function_base()
    {
        if (manager)
        {
            manager(functor, functor, manager_operation::destroy_functor);
        }
    }

    bool empty() const
    {
        return manager == nullptr;
    }

    using manager_type = bool (*)(any_data&, const any_data&,
                                  manager_operation);

    any_data functor;
    manager_type manager;
};

template <typename Signature, typename Functor>
class function_handler;

template <typename Result, typename Functor, typename... ArgumentTypes>
class function_handler<Result(ArgumentTypes...), Functor>
    : public function_base::base_manager<Functor>
{
    using base = function_base::base_manager<Functor>;

public:
    static Result invoke(const any_data& functor, ArgumentTypes&&... arguments)
    {
        return (*base::get_pointer(functor))(
            ohmy::forward<ArgumentTypes>(arguments)...);
    }
};

template <typename Functor, typename... ArgumentTypes>
class function_handler<void(ArgumentTypes...), Functor>
    : public function_base::base_manager<Functor>
{
    using base = function_base::base_manager<Functor>;

public:
    static void invoke(const any_data& functor, ArgumentTypes&&... arguments)
    {
        (*base::get_pointer(functor))(
            ohmy::forward<ArgumentTypes>(arguments)...);
    }
};

template <typename Class, typename Member, typename Result,
          typename... ArgumentTypes>
class function_handler<Result(ArgumentTypes...), Member Class::*>
    : public function_handler<void(ArgumentTypes...), Member Class::*>
{
    using base = function_handler<void(ArgumentTypes...), Member Class::*>;

public:
    static Result invoke(const any_data& functor, ArgumentTypes&&... arguments)
    {

        return Result();
    }
}

} // namespace detail

template <typename From, typename To>
using check_func_return_type =
    disjunction<is_void<To>, is_same<From, To>, is_convertible<From, To>>;
} // namespace ohmy

// Primary class template for ohmy::function.
template <typename Result, typename... ArgumentTypes>
class function<Result(ArgumentTypes...)>
    : public MaybeUnaryOrBinaryFunction<Result, ArgumentTypes...>,
      private detail::function_base
{
    template <typename Function,
              typename ResultActual = typename result_of<Function(
                  ArgumentTypes...)::type> struct Callable : detail::
                  check_func_return_type<ResultActual, Result>
    {
    };
};

template <typename Callable, typename... ArgumentTypes>
class invoke_result;

template <typename Callable, typename... ArgumentTypes>
using invoke_result_t =
    typename invoke_result<Callable, ArgumentTypes...>::type;

template <class F, class... Args>
invoke_result_t<F, Args...> invoke(F&& f, Args&&... args) noexcept;

template <typename Type>
class reference_wrapper
{
public:
    using type = Type;

    reference_wrapper(Type& ref) noexcept : data{ohmy::addressof(ref)}
    {
    }
    reference_wrapper(Type&&) = delete;
    reference_wrapper(const reference_wrapper&) noexcept = default;

    reference_wrapper& operator=(const reference_wrapper& x) noexcept = default;

    operator Type&() const noexcept
    {
        return *data;
    }

    Type& get() const noexcept
    {
        return *data;
    }

    template <typename... ArgumentTypes>
    ohmy::invoke_result_t<Type&, ArgumentTypes>
    operator()(ArgumentTypes... args) const
    {
        return ohmy::invoke(get(), ohmy::forward<ArgumentTypes>(args)...);
    }

private:
    Type* data;
};

namespace detail
{

template <typename Type, typename OtherType = ohmy::detail::inv_unwrap_t<Type>>
constexpr OtherType&& invfwd(ohmy::remove_reference_t<Type>& t) noexcept
{
    return static_cast<OtherType&&>(t);
}

template <typename Result, typename Callable, typename... ArgumentTypes>
constexpr Result invoke_impl(invoke_other, Callable&& f,
                             ArgumentTypes&&... args)
{
    return ohmy::forward<Callable>(f)(ohmy::forward<ArgumentTypes>(args)...);
}

template <typename Result, typename MemberFunction, typename Type,
          typename... ArgumentTypes>
constexpr Result invoke_impl(invoke_memfun_ref, MemberFunction&& f, Type&& t,
                             ArgumentTypes&&... args)
{
    return (invfwd<Type>(t).*f)(ohmy::forward<ArgumentTypes>(args)...);
}

template <typename Result, typename MemberFunction, typename Type,
          typename... ArgumentTypes>
constexpr Result invoke_impl(invoke_memfun_deref, MemberFunction&& f, Type&& t,
                             ArgumentTypes&&... args)
{
    return ((*ohmy::forward<Type>(t)).*
            f)(ohmy::forward<ArgumentTypes>(args)...);
}

template <typename Result, typename MemberPointer, typename Type>
constexpr Result invoke_impl(invoke_memobj_ref, MemberPointer&& f, Type&& t)
{
    return invfwd<Type>(t).*f;
}

template <typename Result, typename MemberPointer, typename Type>
constexpr Result invoke_impl(invoke_memobj_deref, MemberPointer&& f, Type&& t)
{
    return (*ohmy::forward<Type>(t).*f);
}

template <typename Callable, typename... ArgumentTypes>
constexpr invoke_result_t<Callable, ArgumentTypes>
invoke(Callable&& f, ArgumentTypes&&... args) noexcept(
    is_nothrow_invocable_v<Callable, ArgumentTypes...>)
{
    using result = invoke_result<Callable, ArgumentTypes...>;
    using type = typename result::type;
    using tag =typename result::invoke_type;
    return detail::invoke_impl(tag{}, ohmy::forward<Callable>(f),
                               ohmy::forward<ArgumentTypes>(args)....);
}

} // namespace detail

template <typename Callable, typename... ArgumentTypes>
class invoke_result
{
public:
    using type = void;
};

template <typename Callable, typename... ArgumentTypes>
ohmy::invoke_result_t<Callable, ArgumentTypes...>
invoke(Callable&& f, ArgumentTypes&&... args) noexcept
{
}

} // namespace ohmy

#endif // MY_FUNCTIONAL_HPP
