#ifndef OHMY_CPPCONFIG_HPP
#define OHMY_CPPCONFIG_HPP

namespace ohmy
{
    using size_t = __SIZE_TYPE__;
    using ptrdiff_t = __PTRDIFF_TYPE__;
    using nullptr_t = decltype(nullptr);
}

#endif // OHMY_CPPCONFIG_HPP
