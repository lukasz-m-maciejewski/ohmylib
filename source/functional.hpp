#ifndef MY_FUNCTIONAL_HPP
#define MY_FUNCTIONAL_HPP

#include "c++config.hpp"
#include "type_traits.hpp"

namespace ohmy
{

template <typename Type>
inline constexpr bool is_location_invariant_v = is_trivially_copiable_v<Type>;

class UndefinedClass;

union NocopyTypes
{
    void* object;
    const void* const_object;
    void (*functio_pointer)();
    void (UndefinedClass::*member_pointer)();
};

class FunctionBase
{
    static const ohmy::size_t max_size = sizeof(NocopyTypes);
    static const ohmy::size_t max_align = __alignof__(NocopyTypes);

    template <typename Functor>
    class BaseManager
    {
    protected:
        static const bool stored_locally = is_location_invariant_v<Functor>;
    };
};

namespace detail
{
template <typename From, typename To>
using check_func_return_type = disjunction<is_void<To>, is_same<From, To>, is_convertible<From, To>>;
}

// Primary class template for ohmy::function.
template <typename Result, typename... ArgumentTypes>
class function<Result(ArgumentTypes...)>
    : public MaybeUnaryOrBinaryFunction<Result, ArgumentTypes...>,
    private FunctionBase
{
    template <typename Function, typename ResultActual = typename result_of<Function(ArgumentTypes...)::type>
              struct Callable : detail::check_func_return_type<ResultActual, Result> {};

};
}

#endif // MY_FUNCTIONAL_HPP
