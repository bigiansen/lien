SET(LIEN_TEST_PARALLEL_SOURCES
    src/main.cpp
    src/parallel.cpp
    src/parallel_benchmarks.cpp
)

add_executable(lien_test_parallel ${LIEN_TEST_PARALLEL_SOURCES})
target_link_libraries(lien_test_parallel lien_parallel Catch2::Catch2)