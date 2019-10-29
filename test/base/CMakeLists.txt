SET(LIEN_TEST_BASE_SOURCES
	src/aligned_allocator.cpp
    src/main.cpp
)

add_executable(lien_test_base ${LIEN_TEST_BASE_SOURCES})
target_link_libraries(lien_test_base lien_base Catch2::Catch2)