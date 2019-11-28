add_subdirectory(third-party)

find_package(Threads REQUIRED)

set(LIEN_IMAGE_SOURCES
    src/image.cpp
    src/image_ops.cpp
	src/image_unpacked_data.cpp
	src/image_view.cpp
	src/packed_image.cpp
	src/packed_image_view.cpp
	src/internal/std/image_ops_std.cpp	
)

set(LIEN_IMAGE_SOURCES_X86
	src/internal/x86/sse/image_ops_x86.cpp
	src/internal/x86/avx2/image_ops_x86.cpp
)

set(LIEN_IMAGE_SOURCES_ARM
	src/internal/arm/neon/image_ops_neon.cpp
)

if(LIEN_ARCH_X86)
	set(LIEN_IMAGE_SOURCES ${LIEN_IMAGE_SOURCES} ${LIEN_IMAGE_SOURCES_X86})
elseif(LIEN_ARCH_ARM)
	set(LIEN_IMAGE_SOURCES ${LIEN_IMAGE_SOURCES} ${LIEN_IMAGE_SOURCES_ARM})
endif()

file(GLOB LIEN_IMAGE_HEADERS include/ien/*.hpp include/ien/*/*.hpp include/ien/*/*/*.hpp)

add_library(lien_image ${LIEN_IMAGE_SOURCES} ${LIEN_IMAGE_HEADERS})
target_link_libraries(lien_image lien_base stb)
target_include_directories(lien_image PUBLIC include)