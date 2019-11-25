SET(LIEN_PARALLEL_TESTS_SOURCES
    src/main.cpp
    src/parallel.cpp
    src/parallel_benchmarks.cpp
)

add_executable(lien_parallel_tests ${LIEN_PARALLEL_TESTS_SOURCES})
target_link_libraries(lien_parallel_tests lien_parallel Catch2::Catch2)