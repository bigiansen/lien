#include <catch2/catch.hpp>

#include <ien/bit_tools.hpp>

using namespace ien;

TEST_CASE("Bit tools bit extraction")
{
     uint8_t v = 0xF0;

     for(int i = 0; i < 4; ++i)
          REQUIRE(get_bit(v, i) == false);

     for(int i = 4; i < 8; ++i)
          REQUIRE(get_bit(v, i) == true);
};

TEST_CASE("Bit tools bit setting")
{
     SECTION("Set bit")
     {
          for(int i = 0; i < 8; ++i)
          {
               uint8_t v = 0x00;
               set_bit(v, i);
               uint8_t expected = 1 << i;
               REQUIRE(v == expected);
          }
     };

     SECTION("Unset bit")
     {
          for(int i = 0; i < 8; ++i)
          {
               uint8_t v = 0xFF;
               unset_bit(v, i);
               uint8_t expected = ~(1 << i);
               REQUIRE(v == expected);
          }
     };
};

TEST_CASE("Bit tools nibble extraction")
{
    SECTION("Low Nibble")
    {
         uint8_t v = 0x8C;
         uint8_t r = lo_nibble(v);
         REQUIRE(r == 0x0C);
    };

    SECTION("High Nibble")
    {
         uint8_t v = 0x8C;
         uint8_t r = hi_nibble(v);
         REQUIRE(r == 0x08);
    };
};

TEST_CASE("Bit tools byte extraction")
{
    SECTION("Low Byte")
    {
         uint16_t v = 0x8CEF;
         uint8_t r = lo_byte(v);
         REQUIRE(r == 0xEF);
    };

    SECTION("High Byte")
    {
         uint16_t v = 0x8CEF;
         uint8_t r = hi_byte(v);
         REQUIRE(r == 0x8C);
    };
};

TEST_CASE("Bit tools word extraction")
{
    SECTION("Low Word")
    {
         uint32_t v = 0x1234ABCD;
         uint16_t r = lo_word(v);
         REQUIRE(r == 0xABCD);
    };

    SECTION("High Word")
    {
         uint32_t v = 0x1234ABCD;
         uint16_t r = hi_word(v);
         REQUIRE(r == 0x1234);
    };
};

TEST_CASE("Bit tools dword extraction")
{
    SECTION("Low Dword")
    {
         uint64_t v = 0x4A5B6C7D8F8F8F9Ful;
         uint32_t r = lo_dword(v);
         REQUIRE(r == 0x8F8F8F9F);
    };

    SECTION("High Dword")
    {
         uint64_t v = 0x4A5B6C7D8F8F8F9Ful;
         uint32_t r = hi_dword(v);
         REQUIRE(r == 0x4A5B6C7D);
    };
};