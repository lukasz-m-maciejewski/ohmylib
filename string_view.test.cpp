#include <catch/catch.hpp>

#include "string_view.hpp"

TEST_CASE("string_view construction")
{
    SECTION("default-constructed view is empty")
    {
        my::string_view sv{};

        REQUIRE(sv.data() == nullptr);
        REQUIRE(sv.size() == 0u);
        REQUIRE(sv.length() == 0u);
    }

    SECTION("view constructed from literal provides proper length")
    {
        auto str = "abc";
        my::string_view sv{"abc"};

        REQUIRE(sv.data() == str);
        REQUIRE(sv.size() == 3ull);
        REQUIRE(sv.length() == 3ull);

    }

    SECTION("view constructed with explicit length ignores actual lenght of the string")
    {
        auto str = "my string";
        my::string_view sv{str, 20ull};

        REQUIRE(sv.data() == str);
        REQUIRE(sv.size() == 20ull);
        REQUIRE(sv.length() == 20ull);
    }
}

TEST_CASE("begin and end variants")
{
    SECTION("will work with for-each algorithm")
    {
        const char* str = "my example string";
        my::string_view sv{str};

        bool condition_satisfied_acc = true;

        for (auto c : sv)
        {
            condition_satisfied_acc = condition_satisfied_acc and (c == *str++);
        }

        REQUIRE(condition_satisfied_acc);
    }

    SECTION("will work with for loop")
    {
        const char* str = "my example string";
        my::string_view sv{str};

        bool condition_satisfied_acc = true;

        const auto sv_end = sv.end();
        for (auto it = sv.begin(); it != sv_end; ++it, ++str)
        {
            condition_satisfied_acc = condition_satisfied_acc and (*it == *str);
        }

        REQUIRE(condition_satisfied_acc);
    }

    SECTION("will work with for loop and const variants")
    {
        const char* str = "my example string";
        my::string_view sv{str};

        bool condition_satisfied_acc = true;

        const auto sv_end = sv.cend();
        for (auto it = sv.cbegin(); it != sv_end; ++it, ++str)
        {
            condition_satisfied_acc = condition_satisfied_acc and (*it == *str);
        }

        REQUIRE(condition_satisfied_acc);
    }

    SECTION("will iterate in reverse")
    {
        const char* str = "my example string";
        my::string_view sv{str};

        bool condition_satisfied_acc = true;

        const auto sv_rend = sv.crend();
        auto st_rev = str + 16ull;

        for (auto it = sv.crbegin(); it != sv_rend; ++it, --st_rev)
        {
            condition_satisfied_acc = condition_satisfied_acc && (*it == *st_rev);
        }
    }
}
