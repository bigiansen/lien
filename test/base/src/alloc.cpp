#include <catch2/catch.hpp>

#include <ien/alloc.hpp>

TEST_CASE("Aligned alloc")
{
	SECTION("Align 2 -> 4096 bytes, 32K")
	{
		for (size_t i = 2; i < 4096; ++i)
		{
			const uint8_t* data_ptr = ien::aligned_alloc(32768, i);
			uintptr_t data_ptrval = reinterpret_cast<uintptr_t>(data_ptr);
			REQUIRE(data_ptrval % i == 0);
		}
	}

	SECTION("Align more than data length")
	{
		const size_t alignment = 4096;
		const size_t data_len = 1024;
		const uint8_t* data_ptr = ien::aligned_alloc(data_len, alignment);
		uintptr_t data_ptrval = reinterpret_cast<uintptr_t>(data_ptr);
		REQUIRE(data_ptrval % alignment == 0);
	}
}