#ifndef MY_STRING_VIEW
#define MY_STRING_VIEW

#include <algorithm>
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

    static constexpr size_type npos = size_type(-1);

    static_assert(std::is_same<CharT, typename Traits::char_type>::value, "char traits need to refer to base type");

    constexpr basic_string_view() noexcept
        : sz{0u}, data_ptr{nullptr}
    {
    }

    constexpr basic_string_view(const basic_string_view& ) noexcept = default;
    constexpr basic_string_view& operator=(const basic_string_view& ) noexcept = default;

    constexpr basic_string_view(const_pointer c)
        : sz{c == nullptr ? 0ull : traits_type::length(c)}, data_ptr{c}
    {
    }

    constexpr basic_string_view(const_pointer c, size_type n)
        : sz{n}, data_ptr{c}
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

    constexpr void remove_prefix(size_type n) { data_ptr += n; }
    constexpr void remove_suffix(size_type n) { sz -= n; }

    constexpr void swap(basic_string_view& other)
    {
        std::swap(other.data_ptr, data_ptr);
        std::swap(other.sz, sz);
    }

    size_type copy(CharT* dest, size_type count, size_type pos = 0ull) const
    {
        if (pos >= sz)
            throw std::out_of_range(__PRETTY_FUNCTION__);

        auto rcount = std::min(count, sz - pos);
        auto dest_end_it = std::copy_n(data_ptr, rcount, dest);
        return std::distance(dest, dest_end_it);
    }

    basic_string_view substr(size_type pos = 0, size_type count = npos) const
    {
        if (pos >= sz)
            throw std::out_of_range(__PRETTY_FUNCTION__);

        return basic_string_view(data_ptr + pos, std::min(count, sz - pos));
    }

    constexpr int compare(basic_string_view v) const
    {
        return compare(0, npos, v, 0, npos);
    }

    constexpr int compare(size_type pos, size_type count, basic_string_view v) const
    {
        return compare(pos, count, v, 0, npos);
    }

    constexpr int compare(size_type pos1, size_type count1, basic_string_view v,
                          size_type pos2, size_type count2) const
    {
        if (pos1 >= sz or pos2 >= v.sz)
            throw std::out_of_range(__PRETTY_FUNCTION__);

        auto rlen = std::min(std::min(sz - pos1, count1), std::min(v.sz - pos2, count2));
        auto traits_rv = traits_type::compare(data_ptr + pos1, v.data_ptr + pos2, rlen);

        if (traits_rv != 0)
            return traits_rv;
        else
            return sz < v.sz ? -1 : (sz > v.sz ? 1 : 0);
    }

    constexpr int compare(const_pointer s) const
    {
        return compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos, size_type count, const_pointer s) const
    {
        return substr(pos, count).compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos1, size_type count1,
                          const_pointer s, size_type count2)
    {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }

    constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept
    {
        if (sz < v.sz)
            return npos;

        for (auto t_pos = size_type{pos}; t_pos + v.sz <= sz; ++t_pos)
        {
            if (traits_type::compare(data_ptr + t_pos, v.data_ptr, v.sz) == 0)
                return t_pos;
        }

        return npos;
    }

    constexpr size_type find(value_type c, size_type pos = 0) const noexcept
    {
        return find(basic_string_view(&c, 1ull), pos);
    }

    constexpr size_type find(const_pointer s, size_type pos, size_type count) const
    {
        return find(basic_string_view(s, count), pos);
    }

    constexpr size_type find(const_pointer s, size_type pos = 0) const
    {
        return find(basic_string_view(s), pos);
    }

    constexpr size_type rfind(basic_string_view v, size_type pos = npos) const noexcept
    {
        if (sz < v.sz)
            return npos;

        auto t_pos = std::min(sz - v.sz, pos);

        do
        {
            if (traits_type::compare(data_ptr + t_pos, v.data_ptr, v.sz) == 0)
                return t_pos;
        }
        while (t_pos-- > 0);

        return npos;
    }

    constexpr size_type rfind(value_type c, size_type pos = npos) const noexcept
    {
        return rfind(basic_string_view(&c, 1), pos);
    }

    constexpr size_type rfind(const_pointer s, size_type pos, size_type count) const
    {
        return rfind(basic_string_view(s, count), pos);
    }

    constexpr size_type rfind(const_pointer s, size_type pos = npos) const
    {
        return rfind(basic_string_view(s), pos);
    }

    constexpr size_type find_first_of(basic_string_view v, size_type pos = 0) const noexcept
    {
        if (v.sz == size_type{0})
            return npos;

        for (size_type i = pos; i < sz; ++i)
        {
            if (v.find(data_ptr[i]) != npos)
                return i;
        }

        return npos;
    }

    constexpr size_type find_first_of(value_type c, size_type pos = 0) const noexcept
    {
        return find_first_of(basic_string_view{&c, 1ull}, pos);
    }

    constexpr size_type find_first_of(const_pointer s, size_type pos, size_type count) const
    {
        return find_first_of(basic_string_view{s, count}, pos);
    }

    constexpr size_type find_first_of(const_pointer s, size_type pos = 0) const
    {
        return find_first_of(basic_string_view{s}, pos);
    }

    constexpr size_type find_last_of(basic_string_view v, size_type pos = npos) const noexcept
    {
        auto rbegin = std::min(pos, sz);

        if (rbegin == 0)
        {
            if (v.find(data_ptr[rbegin]) != npos)
                return 0;
            else
                return npos;
        }

        do
        {
            if (v.find(data_ptr[rbegin]) != npos)
                return rbegin;
        }
        while (rbegin-- > 0);
    }

private:
    size_type sz;
    const_pointer data_ptr;
};

using string_view = basic_string_view<char>;

}

#endif // MY_STRING_VIEW
