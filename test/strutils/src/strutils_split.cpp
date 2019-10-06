#include <catch2/catch.hpp>
#include <ien/strutils.hpp>

TEST_CASE("split(string)")
{
    SECTION("Common split case")
    {
        std::string str = "1234;4321;0000";
        std::vector<std::string> segments = ien::strutils::split(str, ';');
        REQUIRE(segments.size() == 3);
        REQUIRE(segments[0] == "1234");
        REQUIRE(segments[1] == "4321");
        REQUIRE(segments[2] == "0000");
    }

    SECTION("Only delimiter(s) case")
    {
        std::string str1 = ";";
        std::string str2 = ";;;;;";
        std::vector<std::string> segments1 = ien::strutils::split(str1, ';');
        std::vector<std::string> segments2 = ien::strutils::split(str2, ';');

        REQUIRE(segments1.size() == 0);
        REQUIRE(segments2.size() == 0);
    }

    SECTION("Delimiter at start/end")
    {
        std::string str1 = ";1234;4321;0000";
        std::string str2 = "1234;4321;0000;";
        std::string str3 = ";1234;4321;0000;";

        std::vector<std::string> segments1 = ien::strutils::split(str1, ';');
        std::vector<std::string> segments2 = ien::strutils::split(str2, ';');
        std::vector<std::string> segments3 = ien::strutils::split(str3, ';');

        REQUIRE(segments1.size() == 3);
        REQUIRE(segments2.size() == 3);
        REQUIRE(segments3.size() == 3);

        REQUIRE(segments1[0] == "1234");
        REQUIRE(segments1[1] == "4321");
        REQUIRE(segments1[2] == "0000");

        REQUIRE(segments2[0] == "1234");
        REQUIRE(segments2[1] == "4321");
        REQUIRE(segments2[2] == "0000");

        REQUIRE(segments3[0] == "1234");
        REQUIRE(segments3[1] == "4321");
        REQUIRE(segments3[2] == "0000");
    }

    SECTION("Empty string")
    {
        std::string str = "";
        std::vector<std::string> segments = ien::strutils::split(str, ';');

        REQUIRE(segments.size() == 0);
    }

    SECTION("No delimiters string")
    {
        std::string str = "haha, no split for you!";
        std::vector<std::string> segments = ien::strutils::split(str, ';');
        REQUIRE(segments.size() == 1);
        REQUIRE(segments[0] == "haha, no split for you!");
    }
}

TEST_CASE("split(string_view)")
{
    SECTION("Common split case")
    {
        std::string_view str = "1234;4321;0000";
        std::vector<std::string> segments = ien::strutils::split(str, ';');
        REQUIRE(segments.size() == 3);
        REQUIRE(segments[0] == "1234");
        REQUIRE(segments[1] == "4321");
        REQUIRE(segments[2] == "0000");
    }

    SECTION("Only delimiter(s) case")
    {
        std::string_view str1 = ";";
        std::string_view str2 = ";;;;;";
        std::vector<std::string> segments1 = ien::strutils::split(str1, ';');
        std::vector<std::string> segments2 = ien::strutils::split(str2, ';');

        REQUIRE(segments1.size() == 0);
        REQUIRE(segments2.size() == 0);
    }

    SECTION("Delimiter at start/end")
    {
        std::string_view str1 = ";1234;4321;0000";
        std::string_view str2 = "1234;4321;0000;";
        std::string_view str3 = ";1234;4321;0000;";

        std::vector<std::string> segments1 = ien::strutils::split(str1, ';');
        std::vector<std::string> segments2 = ien::strutils::split(str2, ';');
        std::vector<std::string> segments3 = ien::strutils::split(str3, ';');

        REQUIRE(segments1.size() == 3);
        REQUIRE(segments2.size() == 3);
        REQUIRE(segments3.size() == 3);

        REQUIRE(segments1[0] == "1234");
        REQUIRE(segments1[1] == "4321");
        REQUIRE(segments1[2] == "0000");

        REQUIRE(segments2[0] == "1234");
        REQUIRE(segments2[1] == "4321");
        REQUIRE(segments2[2] == "0000");

        REQUIRE(segments3[0] == "1234");
        REQUIRE(segments3[1] == "4321");
        REQUIRE(segments3[2] == "0000");
    }

    SECTION("Empty string")
    {
        std::string_view str = "";
        std::vector<std::string> segments = ien::strutils::split(str, ';');

        REQUIRE(segments.size() == 0);
    }

    SECTION("No delimiters string")
    {
        std::string_view str = "haha, no split for you!";
        std::vector<std::string> segments = ien::strutils::split(str, ';');
        REQUIRE(segments.size() == 1);
        REQUIRE(segments[0] == "haha, no split for you!");
    }
}

TEST_CASE("split_view(string)")
{
    SECTION("Common split case")
    {
        std::string str = "1234;4321;0000";
        std::vector<std::string_view> segments = ien::strutils::split_view(str, ';');
        REQUIRE(segments.size() == 3);
        REQUIRE(segments[0] == "1234");
        REQUIRE(segments[1] == "4321");
        REQUIRE(segments[2] == "0000");
    }

    SECTION("Only delimiter(s) case")
    {
        std::string str1 = ";";
        std::string str2 = ";;;;;";
        std::vector<std::string_view> segments1 = ien::strutils::split_view(str1, ';');
        std::vector<std::string_view> segments2 = ien::strutils::split_view(str2, ';');

        REQUIRE(segments1.size() == 0);
        REQUIRE(segments2.size() == 0);
    }

    SECTION("Delimiter at start/end")
    {
        std::string str1 = ";1234;4321;0000";
        std::string str2 = "1234;4321;0000;";
        std::string str3 = ";1234;4321;0000;";

        std::vector<std::string_view> segments1 = ien::strutils::split_view(str1, ';');
        std::vector<std::string_view> segments2 = ien::strutils::split_view(str2, ';');
        std::vector<std::string_view> segments3 = ien::strutils::split_view(str3, ';');

        REQUIRE(segments1.size() == 3);
        REQUIRE(segments2.size() == 3);
        REQUIRE(segments3.size() == 3);

        REQUIRE(segments1[0] == "1234");
        REQUIRE(segments1[1] == "4321");
        REQUIRE(segments1[2] == "0000");

        REQUIRE(segments2[0] == "1234");
        REQUIRE(segments2[1] == "4321");
        REQUIRE(segments2[2] == "0000");

        REQUIRE(segments3[0] == "1234");
        REQUIRE(segments3[1] == "4321");
        REQUIRE(segments3[2] == "0000");
    }

    SECTION("Empty string")
    {
        std::string str = "";
        std::vector<std::string_view> segments = ien::strutils::split_view(str, ';');

        REQUIRE(segments.size() == 0);
    }

    SECTION("No delimiters string")
    {
        std::string str = "haha, no split for you!";
        std::vector<std::string_view> segments = ien::strutils::split_view(str, ';');
        REQUIRE(segments.size() == 1);
        REQUIRE(segments[0] == "haha, no split for you!");
    }
}

TEST_CASE("split_view(string_view)")
{
    SECTION("Common split case")
    {
        std::string_view str = "1234;4321;0000";
        std::vector<std::string_view> segments = ien::strutils::split_view(str, ';');
        REQUIRE(segments.size() == 3);
        REQUIRE(segments[0] == "1234");
        REQUIRE(segments[1] == "4321");
        REQUIRE(segments[2] == "0000");
    }

    SECTION("Only delimiter(s) case")
    {
        std::string_view str1 = ";";
        std::string_view str2 = ";;;;;";
        std::vector<std::string_view> segments1 = ien::strutils::split_view(str1, ';');
        std::vector<std::string_view> segments2 = ien::strutils::split_view(str2, ';');

        REQUIRE(segments1.size() == 0);
        REQUIRE(segments2.size() == 0);
    }

    SECTION("Delimiter at start/end")
    {
        std::string_view str1 = ";1234;4321;0000";
        std::string_view str2 = "1234;4321;0000;";
        std::string_view str3 = ";1234;4321;0000;";

        std::vector<std::string_view> segments1 = ien::strutils::split_view(str1, ';');
        std::vector<std::string_view> segments2 = ien::strutils::split_view(str2, ';');
        std::vector<std::string_view> segments3 = ien::strutils::split_view(str3, ';');

        REQUIRE(segments1.size() == 3);
        REQUIRE(segments2.size() == 3);
        REQUIRE(segments3.size() == 3);

        REQUIRE(segments1[0] == "1234");
        REQUIRE(segments1[1] == "4321");
        REQUIRE(segments1[2] == "0000");

        REQUIRE(segments2[0] == "1234");
        REQUIRE(segments2[1] == "4321");
        REQUIRE(segments2[2] == "0000");

        REQUIRE(segments3[0] == "1234");
        REQUIRE(segments3[1] == "4321");
        REQUIRE(segments3[2] == "0000");
    }

    SECTION("Empty string")
    {
        std::string_view str = "";
        std::vector<std::string_view> segments = ien::strutils::split_view(str, ';');

        REQUIRE(segments.size() == 0);
    }

    SECTION("No delimiters string")
    {
        std::string_view str = "haha, no split for you!";
        std::vector<std::string_view> segments = ien::strutils::split_view(str, ';');
        REQUIRE(segments.size() == 1);
        REQUIRE(segments[0] == "haha, no split for you!");
    }
}