set(LIEN_IMAGE_TESTS_SOURCES
    src/image_ops.cpp
    src/main.cpp
)

set(LIEN_IMAGE_TESTS_SOURCES_BENCHMARKS
    src/benchmarks/image_ops_arm_benchmarks.cpp
)

set(LIEN_IMAGE_TESTS_SOURCES_X86
    src/x86/image_ops_x86.cpp
)

set(LIEN_IMAGE_TESTS_SOURCES_ARM
    src/arm/image_ops_arm.cpp
)

if(LIEN_ARCH_X86)
    set(LIEN_IMAGE_TESTS_SOURCES ${LIEN_IMAGE_TESTS_SOURCES} ${LIEN_IMAGE_TESTS_SOURCES_X86})
elseif(LIEN_ARCH_ARM)
    set(LIEN_IMAGE_TESTS_SOURCES ${LIEN_IMAGE_TESTS_SOURCES} ${LIEN_IMAGE_TESTS_SOURCES_ARM})
endif()

if(LIEN_BUILD_TESTS_BENCHMARKS)
    set(LIEN_IMAGE_TESTS_SOURCES ${LIEN_IMAGE_TESTS_SOURCES} ${LIEN_IMAGE_TESTS_SOURCES_BENCHMARKS})
    add_compile_definitions(LIEN_BENCHMARK)
endif()

add_executable(lien_image_tests ${LIEN_IMAGE_TESTS_SOURCES})
target_link_libraries(lien_image_tests lien_image Catch2::Catch2)