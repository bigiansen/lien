#pragma once

#include <type_traits>

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

	template<typename T, typename TArg0, typename TArg1>
	constexpr T construct2(TArg0 a0, TArg1 a1)
	{
		static_assert(std::is_arithmetic_v<T>, "Not an arithmetic type");
		static_assert(std::is_arithmetic_v<TArg0>, "Not an arithmetic type");
		static_assert(std::is_arithmetic_v<TArg1>, "Not an arithmetic type");
		static_assert(sizeof(TArg0) + sizeof(TArg1) == sizeof(T), "Size sum does not match target type's size");

		return (static_cast<T>(a0) << (sizeof(T) - sizeof(TArg0))) | static_cast<T>(a1);
	}

	template<typename T, typename TArg0, typename TArg1, typename TArg2, typename TArg3>
	constexpr T construct4(TArg0 a0, TArg1 a1, TArg2 a2, TArg3 a3)
	{
		static_assert(std::is_arithmetic_v<T>, "Not an arithmetic type");
		static_assert(std::is_arithmetic_v<TArg0>, "Not an arithmetic type");
		static_assert(std::is_arithmetic_v<TArg1>, "Not an arithmetic type");
		static_assert(std::is_arithmetic_v<TArg2>, "Not an arithmetic type");
		static_assert(std::is_arithmetic_v<TArg3>, "Not an arithmetic type");
		static_assert(sizeof(TArg0) + sizeof(TArg1) + sizeof(TArg2) + sizeof(TArg3) == sizeof(T), "Size sum does not match target type's size");

		return (static_cast<T>(a0) << (sizeof(T) - sizeof(TArg0)))
			| (static_cast<T>(a1) << (sizeof(T) - sizeof(TArg0) - sizeof(TArg1)))
			| (static_cast<T>(a2) << (sizeof(T) - sizeof(TArg0) - sizeof(TArg1) - sizeof(TArg2)))
			| static_cast<T>(a3);
	}
}