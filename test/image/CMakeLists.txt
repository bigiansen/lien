SET(LIEN_IMAGE_TESTS_SOURCES
    src/image_ops.cpp
    src/x86/image_ops.cpp
    src/arm/image_ops.cpp
    src/benchmarks/image_ops.cpp
    src/main.cpp
)

add_executable(lien_image_tests ${LIEN_IMAGE_TESTS_SOURCES})
target_link_libraries(lien_image_tests lien_image Catch2::Catch2)
