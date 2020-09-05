#include <ien/alloc.hpp>

namespace ien::_internal
{
	std::unordered_map<void*, void*> _ptr_table;

	void* aligned_alloc(size_t bytes, size_t alignment)
	{
		void* ptr = malloc(bytes + (alignment - 1));
		uintptr_t ptrval = reinterpret_cast<uintptr_t>(ptr);
		const auto misalignment = (alignment - (ptrval % alignment)) % alignment;
		void* result = reinterpret_cast<void*>(ptrval + misalignment);
		_ptr_table.emplace(result, ptr);
		return result;
	}

	void aligned_free(void* ptr)
	{
		free(_ptr_table[ptr]);
		_ptr_table.erase(ptr);
	}
}