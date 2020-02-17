#pragma once

namespace ien
{
	template<typename T, typename ... TArgs>
	constexpr T safe_add(TArgs... args)
	{
		static_assert(std::is_arithmetic_v<T>, "Not an arithmetic type");
		return (static_cast<T>(args) + ...);
	}

	template<typename T, typename ... TArgs>
	constexpr T safe_mul(TArgs... args)
	{
		static_assert(std::is_arithmetic_v<T>, "Not an arithmetic type");
		return (static_cast<T>(args) * ...);
	}
}