#include <catch2/catch.hpp>

#include <ien/bit_iterator.hpp>
#include <array>
#include <cinttypes>

template<typename TData, size_t Length>
std::array<TData, Length> get_rand_array()
{
	std::array<TData, Length> result;
	for (size_t i = 0; i < Length; ++i)
	{
		result[i] = static_cast<uint8_t>(rand());
	}
	return result;
}

TEST_CASE("Bit iterator size")
{
	SECTION("0 Elements")
	{
		const unsigned char data[] = { 0 };
		ien::bit_iterator iter(data, 0);

		REQUIRE(iter.bitsize() == 0);
	}

	SECTION("10 elements, 8bit per element")
	{
		const std::array<uint8_t, 10> data = get_rand_array<uint8_t, 10>();
		ien::bit_iterator iter(data.data(), data.size());
		REQUIRE(iter.bitsize() == data.size() * sizeof(uint8_t));
	}

	SECTION("10 elements, 16bit per element")
	{
		const std::array<uint16_t, 10> data = get_rand_array<uint16_t, 10>();
		ien::bit_iterator iter(data.data(), data.size());
		REQUIRE(iter.bitsize() == data.size() * sizeof(uint16_t));
	}
}

TEST_CASE("Bit iterator values")
{
	SECTION("All zeroes (MSB_TO_LSB)")
	{
		const unsigned char data[] = { 0, 0, 0, 0 };
		ien::bit_iterator iter(data, sizeof(data));

		for (size_t i = 0; i < iter.bitsize(); ++i)
		{
			REQUIRE(iter.next() == false);
		}
	}

	SECTION("All ones (MSB_TO_LSB)")
	{
		const unsigned char data[] = { 0xFF, 0xFF, 0xFF, 0xFF };
		ien::bit_iterator iter(data, sizeof(data));

		for (size_t i = 0; i < iter.bitsize(); ++i)
		{
			REQUIRE(iter.next() == true);
		}
	}

	SECTION("All zeroes (LSB_TO_MSB)")
	{
		const uint8_t data[] = { 0, 0, 0, 0 };
		ien::bit_iterator<uint8_t, ien::bit_iterator_mode::LSB_TO_MSB> iter(data, sizeof(data));

		for (size_t i = 0; i < iter.bitsize(); ++i)
		{
			REQUIRE(iter.next() == false);
		}
	}

	SECTION("All ones (LSB_TO_MSB)")
	{
		const uint8_t data[] = { 0xFF, 0xFF, 0xFF, 0xFF };
		ien::bit_iterator<uint8_t, ien::bit_iterator_mode::LSB_TO_MSB> iter(data, sizeof(data));

		for (size_t i = 0; i < iter.bitsize(); ++i)
		{
			REQUIRE(iter.next() == true);
		}
	}

	SECTION("Specific (MSB_TO_LSB)")
	{
		const uint8_t data[] = { 0b11110000, 0b00001111, 0xFF, 0x00 };
		ien::bit_iterator<uint8_t, ien::bit_iterator_mode::MSB_TO_LSB> iter(data, sizeof(data));

		for (size_t i = 0; i < 4; ++i)   
			REQUIRE(iter.next() == true);
		for (size_t i = 4; i < 12; ++i)  
			REQUIRE(iter.next() == false);
		for (size_t i = 12; i < 24; ++i) 
			REQUIRE(iter.next() == true);
		for (size_t i = 24; i < 32; ++i) 
			REQUIRE(iter.next() == false);
	}

	SECTION("Specific (LSB_TO_MSB)")
	{
		const uint8_t data[] = { 0b11110000, 0b00001111, 0xFF, 0x00 };
		ien::bit_iterator iter(data, sizeof(data));

		for (size_t i = 0; i < 4; ++i)   
			REQUIRE(iter.next() == false);
		for (size_t i = 4; i < 12; ++i)  
			REQUIRE(iter.next() == true);
		for (size_t i = 12; i < 16; ++i) 
			REQUIRE(iter.next() == false);
		for (size_t i = 16; i < 24; ++i) 
			REQUIRE(iter.next() == true);
		for (size_t i = 24; i < 32; ++i) 
			REQUIRE(iter.next() == false);
	}
}