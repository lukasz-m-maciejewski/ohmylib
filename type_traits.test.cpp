#include "type_traits.hpp"

static_assert(ohmy::true_type::value == true, "");
static_assert(ohmy::false_type::value == false, "");

static_assert(ohmy::is_same<int, int>::value == true, "");
static_assert(ohmy::is_same<int, float>::value == false, "");
static_assert(ohmy::is_same_v<int, int> == true, "");
static_assert(ohmy::is_same_v<int, float> == false, "");

struct A
{
    void foo();
};

void bar();

struct B
{
};

struct D : public B
{
};

static_assert(ohmy::is_base_of<D, B>::value == false, "");
static_assert(ohmy::is_base_of<B, D>::value == true, "");
static_assert(ohmy::is_base_of<B, A>::value == false, "");
static_assert(ohmy::is_base_of_v<D, B> == false, "");
static_assert(ohmy::is_base_of_v<B, D> == true, "");
static_assert(ohmy::is_base_of_v<B, A> == false, "");

static_assert(ohmy::is_same_v<typename ohmy::conditional<true, int, float>::type, int>, "");
static_assert(ohmy::is_same_v<typename ohmy::conditional<false, int, float>::type, float>, "");
static_assert(ohmy::is_same_v<ohmy::conditional_t<true, int, float>, int>, "");
static_assert(ohmy::is_same_v<ohmy::conditional_t<false, int, float>, float>, "");

static_assert(ohmy::is_same_v<typename ohmy::remove_const<int>::type, int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_const_t<int>, int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_const<const int>::type, int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_const_t<const int>, int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_const<volatile int>::type, volatile int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_const_t<volatile int>, volatile int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_const<const volatile int>::type, volatile int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_const_t<const volatile int>, volatile int>, "");

static_assert(ohmy::is_same_v<typename ohmy::remove_volatile<int>::type, int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_volatile_t<int>, int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_volatile<const int>::type, const int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_volatile_t<const int>, const int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_volatile<volatile int>::type, int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_volatile_t<volatile int>, int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_volatile<const volatile int>::type, const int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_volatile_t<const volatile int>, const int>, "");

static_assert(ohmy::is_same_v<typename ohmy::remove_cv<int>::type, int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_cv_t<int>, int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_cv<const int>::type, int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_cv_t<const int>, int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_cv<volatile int>::type, int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_cv_t<volatile int>, int>, "");
static_assert(ohmy::is_same_v<typename ohmy::remove_cv<const volatile int>::type, int>, "");
static_assert(ohmy::is_same_v<ohmy::remove_cv_t<const volatile int>, int>, "");

static_assert(ohmy::is_const<int>::value == false, "");
static_assert(ohmy::is_const<const A>::value == true, "");

static_assert(ohmy::is_lvalue_reference<int>::value == false, "");
static_assert(ohmy::is_lvalue_reference<int&>::value == true, "");
static_assert(ohmy::is_lvalue_reference<int&&>::value == false, "");

static_assert(ohmy::is_rvalue_reference<int>::value == false, "");
static_assert(ohmy::is_rvalue_reference<int&>::value == false, "");
static_assert(ohmy::is_rvalue_reference<int&&>::value == true, "");

static_assert(ohmy::is_reference<int>::value == false, "");
static_assert(ohmy::is_reference<int&>::value == true, "");
static_assert(ohmy::is_reference<int&&>::value == true, "");

static_assert(ohmy::is_function<int>::value == false, "");
//static_assert(ohmy::is_function<decltype(&A::foo)>::value == true, "");
static_assert(ohmy::is_function<decltype(bar)>::value == true, "");

static_assert(ohmy::is_array<int>::value == false, "");
static_assert(ohmy::is_array_v<int> == false, "");
static_assert(ohmy::is_array<int[4]>::value == true, "");
static_assert(ohmy::is_array_v<int[4]> == true, "");
static_assert(ohmy::is_array<int[]>::value == true, "");
static_assert(ohmy::is_array_v<int[]> == true, "");

static_assert(ohmy::is_object_v<int> == true);
static_assert(ohmy::is_object_v<float&> == false);

static_assert(ohmy::is_same_v<ohmy::add_lvalue_reference<int>::type, int&>);
static_assert(ohmy::is_same_v<ohmy::add_lvalue_reference_t<int>, int&>);

static_assert(ohmy::is_same_v<ohmy::add_rvalue_reference<int>::type, int&&>);
static_assert(ohmy::is_same_v<ohmy::add_rvalue_reference_t<int>, int&&>);

static_assert(ohmy::is_convertible<int, float>::value == true);
static_assert(ohmy::is_convertible_v<int, float> == true);
static_assert(ohmy::is_convertible<float, int>::value == true);
static_assert(ohmy::is_convertible_v<float, int> == true);

static_assert(ohmy::is_convertible<int(*)(int), double*>::value == false);
static_assert(ohmy::is_convertible_v<int(*)(int), double*> == false);

struct AA
{

};

struct BB
{
    operator AA ();
};

struct CC
{
    CC(const AA&);
};

struct DD
{
};

static_assert(ohmy::is_convertible_v<AA, BB> == false);
static_assert(ohmy::is_convertible_v<BB, AA> == true);

static_assert(ohmy::is_convertible_v<AA, CC> == true);
static_assert(ohmy::is_convertible_v<CC, AA> == false);

static_assert(ohmy::is_convertible_v<BB, CC> == false);
static_assert(ohmy::is_convertible_v<CC, BB> == false);

static_assert(ohmy::is_convertible_v<AA, DD> == false);
static_assert(ohmy::is_convertible_v<DD, AA> == false);

static_assert(ohmy::is_convertible_v<AA, AA> == true);

static_assert(ohmy::is_same_v<ohmy::remove_all_extents_t<int[][1][123]>, int>);

enum class Fake
{
    This,
    That
};

static_assert(ohmy::is_arithmetic_v<float> == true);
static_assert(ohmy::is_arithmetic_v<float*> == false);
static_assert(ohmy::is_arithmetic_v<int> == true);
static_assert(ohmy::is_arithmetic_v<BB> == false);
static_assert(ohmy::is_arithmetic_v<Fake> == false);
static_assert(ohmy::is_arithmetic_v<int[]> == false);
static_assert(ohmy::is_arithmetic_v<int[4]> == false);
