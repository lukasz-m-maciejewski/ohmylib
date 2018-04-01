#ifndef MY_STRING_VIEW
#define MY_STRING_VIEW


#include <iterator>
#include <limits>
#include <string>

namespace my
{

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view
{
public:
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = CharT*;
    using const_pointer = const CharT*;
    using reference = CharT&;
    using const_reference = const CharT&;
    using const_iterator = const_pointer;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static_assert(std::is_same<CharT, typename Traits::char_type>::value, "char traits need to refer to base type");

    constexpr basic_string_view()
        : data_ptr{nullptr}, sz{0u}
    {
    }

    constexpr basic_string_view(const basic_string_view& ) = default;
    constexpr basic_string_view& operator=(const basic_string_view& ) = default;

    constexpr basic_string_view(const_pointer c)
        : data_ptr{c}, sz{traits_type::length(c)}
    {
    }

    constexpr basic_string_view(const_pointer c, size_type n)
        : data_ptr{c}, sz{n}
    {
    }

    constexpr const_iterator cbegin() const { return data_ptr; }
    constexpr const_iterator cend() const { return data_ptr + sz; }

    constexpr const_iterator begin() const { return cbegin(); }
    constexpr const_iterator end() const { return cend(); }

    constexpr const_reverse_iterator crbegin() const { return std::make_reverse_iterator(cend()); }
    constexpr const_reverse_iterator crend() const { return std::make_reverse_iterator(cbegin()); }

    constexpr const_reverse_iterator rbegin() const { return crbegin(); }
    constexpr const_reverse_iterator rend() const { return rend(); }

    constexpr const_reference operator[](size_type pos) const { return *(data_ptr + pos); }
    constexpr const_reference at(size_type pos) const
    {
        if (pos < sz)
            return *(data_ptr + pos);
        else
            throw std::out_of_range{__PRETTY_FUNCTION__};
    }

    constexpr const_reference front() const { return *(data_ptr); }
    constexpr const_reference back() const { return *(data_ptr + sz - 1); }

    constexpr const_pointer data() const { return data_ptr; }

    constexpr size_type size() const { return sz; }
    constexpr size_type length() const { return sz; }

    constexpr size_type max_size() const { return std::numeric_limits<size_type>::max(); }
    constexpr bool empty() const { return (sz == 0ull); }



private:
    const_pointer data_ptr;
    size_type sz;
};

using string_view = basic_string_view<char>;

}

#endif // MY_STRING_VIEW
