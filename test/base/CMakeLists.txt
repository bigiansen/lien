SET(LIEN_BASE_TESTS_SOURCES	
	src/aligned_allocator.cpp
	src/alloc.cpp
	src/arithmetic.cpp
	src/bit_iterator.cpp
	src/bit_tools.cpp
	src/fixed_vector.cpp
	src/main.cpp
)

add_executable(lien_base_tests ${LIEN_BASE_TESTS_SOURCES})
target_link_libraries(lien_base_tests lien_base Catch2::Catch2)