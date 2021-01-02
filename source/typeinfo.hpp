#ifndef MY_TYPEINFO_HPP
#define MY_TYPEINFO_HPP

namespace ohmy
{
class type_info
{
public:
    virtual ~type_info();

    const char* name() const noexcept
    {
        return __name[0] == '*' ? __name + 1 : __name;
    }

protected:
    const char* __name;

    explicit type_info(const char* __n) : __name(__n)
    {
    }
};
}


#endif
