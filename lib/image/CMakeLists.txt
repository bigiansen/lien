add_subdirectory(third-party)

find_package(Threads REQUIRED)

SET(LIEN_IMAGE_SOURCES
    src/image.cpp
    src/image_ops.cpp
	src/image_unpacked_data.cpp
	src/image_view.cpp
	src/packed_image.cpp
	src/packed_image_view.cpp
	src/internal/std/image_ops_std.cpp
	src/internal/x86/sse/image_ops_x86.cpp
	src/internal/x86/avx2/image_ops_x86.cpp
	src/internal/arm/image_ops_arm.cpp
)

FILE(GLOB LIEN_IMAGE_HEADERS include/ien/*.hpp include/ien/*/*.hpp include/ien/*/*/*.hpp)

add_library(lien_image ${LIEN_IMAGE_SOURCES} ${LIEN_IMAGE_HEADERS})
target_link_libraries(lien_image lien_base stb)
target_include_directories(lien_image PUBLIC include)
