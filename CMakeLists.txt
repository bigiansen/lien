cmake_minimum_required (VERSION "3.7.0")

project(lien LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY 			${PROJECT_BINARY_DIR}/output/lib/shared)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY 			${PROJECT_BINARY_DIR}/output/lib/static)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 			${PROJECT_BINARY_DIR}/output/bin/lien)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG 	${PROJECT_BINARY_DIR}/output/lib/shared)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 	${PROJECT_BINARY_DIR}/output/lib/static)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${PROJECT_BINARY_DIR}/output/bin/lien)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE 	${PROJECT_BINARY_DIR}/output/lib/shared)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE 	${PROJECT_BINARY_DIR}/output/lib/static)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	${PROJECT_BINARY_DIR}/output/bin/lien)

option(LIEN_BUILD_TESTS "Build Tests" On)

# Conditional library compilation options
option(LIEN_PARALLEL "Build Parallel utilities library" On)
option(LIEN_IMAGE "Build Image utilities library" On)
option(LIEN_ARM_ENABLE_NEON "Enable NEON code generation on ARM platforms" On)

# x86 FLAG
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    set (LIEN_ARCH_X86 TRUE)
else ()
    set (LIEN_ARCH_X86 FALSE)
endif ()

# ARM FLAG
if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm|ARM|aarch64|AARCH64")
    set (LIEN_ARCH_ARM TRUE)
else ()
    set (LIEN_ARCH_ARM FALSE)
endif ()

# Enable AVX2 code generation on x86 platforms
if(LIEN_ARCH_X86)
    if(MSVC)
        add_definitions(/arch:AVX2)
    else()
        add_definitions(-mavx2)
    endif()
endif()

# Enable NEON code generation on ARM platforms, if enabled by LIEN_ARM_ENABLE_NEON
if(LIEN_ARCH_ARM AND LIEN_ARM_ENABLE_NEON)
    add_definitions(-mfpu=neon -mfloat-abi=softfp)
endif()

add_subdirectory("lib")

if(LIEN_BUILD_TESTS)
    add_subdirectory("third-party")
    add_subdirectory("test")
endif()