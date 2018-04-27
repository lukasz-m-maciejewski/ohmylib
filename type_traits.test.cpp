#include "type_traits.hpp"

static_assert(ohmy::true_type::value == true, "");
static_assert(ohmy::false_type::value == false, "");

static_assert(ohmy::is_same<int, int>::value == true, "");
static_assert(ohmy::is_same<int, float>::value == false, "");

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