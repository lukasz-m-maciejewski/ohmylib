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
        REQUIRE(sv.empty() == true);
    }

    SECTION("view constructed from literal provides proper length")
    {
        auto str = "abc";
        my::string_view sv{"abc"};

        REQUIRE(sv.data() == str);
        REQUIRE(sv.size() == 3ull);
        REQUIRE(sv.length() == 3ull);
        REQUIRE(sv.empty() == false);
    }

    SECTION("view constructed with explicit length ignores actual lenght of the string")
    {
        auto str = "my string";
        my::string_view sv{str, 20ull};

        REQUIRE(sv.data() == str);
        REQUIRE(sv.size() == 20ull);
        REQUIRE(sv.length() == 20ull);
        REQUIRE(sv.empty() == false);
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

        REQUIRE(condition_satisfied_acc);
    }
}

TEST_CASE("will allow read only access to elements")
{
    my::string_view sv("abcdefghijklmnopqrstuwxyz");
    SECTION("will grant unchecked access via operator[]")
    {
        REQUIRE(sv[10] == 'k');
        char out_of_bounds{};
        REQUIRE_NOTHROW(out_of_bounds = sv[123]);

        (void)out_of_bounds;
    }

    SECTION("will grant bounds-checked access via at function")
    {
        REQUIRE(sv.at(15) == 'p');
        char out_of_bounds{};
        REQUIRE_THROWS_AS(out_of_bounds = sv.at(123), std::out_of_range);

        (void)out_of_bounds;
    }

    SECTION("will grant access to first and last element by dedicated functions")
    {
        REQUIRE(sv.front() == 'a');
        REQUIRE(sv.back() == 'z');
    }
}

TEST_CASE("will allow comparing views")
{
    SECTION("when comparing strings of equal length comparison is lexicographic")
    {
        my::string_view sv1{"aaa"};
        my::string_view sv2("aba");
        auto result_1_2 = sv1.compare(sv2);
        auto result_2_1 = sv2.compare(sv1);
        REQUIRE(result_1_2 < 0);
        REQUIRE(result_2_1 > 0);
    }

    SECTION("when comparing strings of different length with same common prefix then longer one is greater")
    {
        my::string_view sv1{"aaa"};
        my::string_view sv2{"aaaa"};

        auto result_1_2 = sv1.compare(sv2);
        auto result_2_1 = sv2.compare(sv1);

        REQUIRE(result_1_2 < 0);
        REQUIRE(result_2_1 > 0);
    }

    SECTION("when comparing strings of different length with different prefixes then prefix determines comparison")
    {
        my::string_view sv1{"aaa"};
        my::string_view sv2{"abab"};

        auto result_1_2 = sv1.compare(sv2);
        auto result_2_1 = sv2.compare(sv1);

        REQUIRE(result_1_2 < 0);
        REQUIRE(result_2_1 > 0);
    }

    SECTION("string view compares with c-string")
    {
        my::string_view sv {"aaaa"};
        auto cstr = std::make_unique<char[]>(5);
        cstr.get()[0] = 'a';
        cstr.get()[1] = 'a';
        cstr.get()[2] = 'a';
        cstr.get()[3] = 'a';
        cstr.get()[4] = 0;

        auto result = sv.compare(cstr.get());
        REQUIRE(result == 0);
    }
}
