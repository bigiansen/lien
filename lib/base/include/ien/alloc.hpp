#pragma once

#include <cinttypes>
#include <cstdlib>

#include <unordered_map>

namespace ien::_internal
{
	[[nodiscard]] void* aligned_alloc(size_t bytes, size_t alignment);
	void aligned_free(void* ptr);
}

namespace ien
{
	template<typename T = uint8_t>
	[[nodiscard]] T* aligned_alloc(size_t len, size_t alignment)
	{
		return reinterpret_cast<T*>(_internal::aligned_alloc(len * sizeof(T), alignment));
	}

	template<typename T>
	void aligned_free(T* ptr)
	{
		_internal::aligned_free(reinterpret_cast<void*>(ptr));
	}

	template<typename T>
	[[nodiscard]] T* aligned_realloc(T* ptr, size_t len, size_t alignment)
	{
		aligned_free(ptr);
		return reinterpret_cast<T*>(aligned_alloc(len, alignment));
	}
}