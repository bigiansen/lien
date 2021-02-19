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

		return (static_cast<T>(a0) << ((sizeof(T) - sizeof(TArg0)) * 8)) | static_cast<T>(a1);
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

		return (static_cast<T>(a0) << ((sizeof(T) - sizeof(TArg0)) * 8))
			| (static_cast<T>(a1) << ((sizeof(T) - sizeof(TArg0) - sizeof(TArg1)) * 8))
			| (static_cast<T>(a2) << ((sizeof(T) - sizeof(TArg0) - sizeof(TArg1) - sizeof(TArg2)) * 8))
			| static_cast<T>(a3);
	}

	template<typename T> 
	struct superior_integral
	{ 
		static_assert(std::is_integral_v<T>, "Not an integral type");
		typedef T type;
	};
	template<> struct superior_integral<uint8_t>	{ typedef uint16_t type; };
	template<> struct superior_integral<uint16_t>	{ typedef uint32_t type; };
	template<> struct superior_integral<uint32_t>	{ typedef uint64_t type; };
	template<> struct superior_integral<int8_t>	{ typedef int16_t  type; };
	template<> struct superior_integral<int16_t>	{ typedef int32_t  type; };
	template<> struct superior_integral<int32_t>	{ typedef int64_t  type; };

	template<typename T>
	using superior_integral_t = typename superior_integral<T>::type;

	template<typename TRet, typename ... TArgs>
	constexpr TRet average(const TArgs& ...args)
	{
		constexpr auto argc = sizeof...(TArgs);
		if constexpr (std::is_integral_v<TRet>)
		{
			superior_integral_t<TRet> result = (args + ...);
			return static_cast<TRet>(result / argc);
		}
		else
		{
			return ((args + ...) / argc);
		}
	}
}