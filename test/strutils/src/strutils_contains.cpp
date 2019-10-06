#include <catch2/catch.hpp>
#include <ien/strutils.hpp>

TEST_CASE("contains(string,char)")
{
    SECTION("Common use case")
    {
        std::string str = "do i have a \'%\' sign?";
        bool found_a = ien::strutils::contains(str, '%');
        bool found_b = ien::strutils::contains(str, '?');
        bool found_c = ien::strutils::contains(str, ' ');
        bool found_d = ien::strutils::contains(str, '$');
        REQUIRE(found_a);
        REQUIRE(found_b);
        REQUIRE(found_c);
        REQUIRE(!found_d);
    }

    SECTION("Empty string")
    {
        std::string str = "";
        bool found = ien::strutils::contains(str, '1');
        REQUIRE(!found);
    }
}