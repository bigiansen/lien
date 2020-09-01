cmake_minimum_required (VERSION "3.7.0")

project(lien LANGUAGES CXX C)

include(cmake/lien.cmake)

LIEN_SET_SHARED_LIB_OUTPUT_DIR(${PROJECT_BINARY_DIR}/output/lib/shared)
LIEN_SET_STATIC_LIB_OUTPUT_DIR(${PROJECT_BINARY_DIR}/output/lib/static)
LIEN_SET_EXECUTABLE_OUTPUT_DIR(${PROJECT_BINARY_DIR}/output/lib/lien)

# -- OPTIONS --
option(LIEN_BUILD_STRUTILS "Build String utilities library" ON)
option(LIEN_BUILD_PARALLEL "Build Parallel utilities library" ON)
option(LIEN_BUILD_IMAGE "Build Image utilities library" ON)
option(LIEN_USE_CUSTOM_SIMD "Use custom SIMD implementations" ON)

option(LIEN_BUILD_TESTS "Build Tests" ON)
option(LIEN_BUILD_TESTS_BENCHMARKS "Build Benchmarks" ON)

option(LIEN_ARM_ENABLE_NEON "Enable NEON code generation on ARM platforms" ON)

if(LIEN_USE_CUSTOM_SIMD)
    add_compile_definitions(LIEN_USE_CUSTOM_SIMD)
endif()

# Enable SSE/AVX code generation on X86 platforms
# This should go in a toolchain, but will remain here for the moment
if(LIEN_ARCH_X86 AND NOT EMSCRIPTEN)
    if(MSVC)
        add_definitions(/arch:AVX2)    
    else()
        add_definitions(-mavx2)
    endif()
elseif(EMSCRIPTEN)
    add_definitions(-msimd128)
endif()

# Enable NEON code generation on ARM platforms, if enabled by the LIEN_ARM_ENABLE_NEON option
# This should go in a toolchain, but will remain here for the moment
if(LIEN_ARCH_ARM AND LIEN_ARM_ENABLE_NEON)
    message("########## ARM NEON ENABLED! ###########")
    add_definitions(-mfpu=neon -mfloat-abi=hard -march=armv7-a -funsafe-math-optimizations)
	add_compile_definitions(LIEN_ARM_NEON)
endif()

LIEN_INIT_GIT_SUBMODULES()

add_subdirectory("lib")

if(LIEN_BUILD_TESTS)
    add_subdirectory("third-party")
    add_subdirectory("test")
endif()