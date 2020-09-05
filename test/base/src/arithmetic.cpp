#include <catch2/catch.hpp>

#include <ien/arithmetic.hpp>
#include <cinttypes>

TEST_CASE("Safe add")
{
	SECTION("u8 to u16")
	{
		const uint8_t a = 0xFF;
		const uint8_t b = 0x01;

		constexpr uint16_t sum = ien::safe_add<uint16_t>(a, b);
		REQUIRE(sum == 0x0100);
	}

	SECTION("u16 to u32")
	{
		const uint16_t a = 0xFFFF;
		const uint16_t b = 0x0001;

		constexpr uint32_t sum = ien::safe_add<uint32_t>(a, b);
		REQUIRE(sum == 0x00010000);
	}

	SECTION("u16 to u8")
	{
		const uint16_t a = 0x00FF;
		const uint16_t b = 0x0001;

		constexpr uint8_t sum = ien::safe_add<uint8_t>(a, b);
		REQUIRE(sum == 0x00);
	}
}

TEST_CASE("Safe mul")
{
	SECTION("u8 to u16")
	{
		const uint8_t a = 200;
		const uint8_t b = 2;

		constexpr uint16_t pro = ien::safe_mul<uint16_t>(a, b);
		REQUIRE(pro == 400);
	}

	SECTION("u16 to u32")
	{
		const uint16_t a = 60000;
		const uint16_t b = 3;

		constexpr uint32_t pro = ien::safe_mul<uint32_t>(a, b);
		REQUIRE(pro == 180000);
	}
}

TEST_CASE("Construct2")
{
	SECTION("2xu8 -> u16")
	{
		const uint8_t a = 0x24;
		const uint8_t b = 0xA7;

		constexpr uint16_t res = ien::construct2<uint16_t>(a, b);
		REQUIRE(res == 0x24A7);
	}

	SECTION("2xu16 -> u32")
	{
		const uint16_t a = 0x24A7;
		const uint16_t b = 0xBC0E;

		constexpr uint32_t res = ien::construct2<uint32_t>(a, b);
		REQUIRE(res == 0x24A7BC0E);
	}

	SECTION("2xu32 -> u64")
	{
		const uint32_t a = 0x24A7BC0E;
		const uint32_t b = 0x1356ADF8;

		constexpr uint64_t res = ien::construct2<uint64_t>(a, b);
		REQUIRE(res == 0x24A7BC0E1356ADF8);
	}
}

TEST_CASE("Construct4")
{
	SECTION("4xu8 -> u32")
	{
		const uint8_t a = 0x24;
		const uint8_t b = 0xA7;
		const uint8_t c = 0x0B;
		const uint8_t d = 0x8E;

		constexpr uint32_t res = ien::construct4<uint32_t>(a, b, c, d);
		REQUIRE(res == 0x24A70B8E);
	}

	SECTION("4xu16 -> u64")
	{
		const uint16_t a = 0x24A7;
		const uint16_t b = 0x0B8E;
		const uint16_t c = 0x701A;
		const uint16_t d = 0xEFA8;

		constexpr uint64_t res = ien::construct4<uint64_t>(a, b, c, d);
		REQUIRE(res == 0x24A70B8E701AEFA8);
	}
}