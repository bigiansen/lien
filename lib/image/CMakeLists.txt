add_subdirectory(third-party)

SET(LIEN_IMAGE_SOURCES
    src/image.cpp
	src/image_ops.cpp
    src/pixel_trait.cpp)

FILE(GLOB LIEN_IMAGE_HEADERS include/ien/*.hpp)

if(MSVC)
	add_definitions(/std:c++17)
endif()

add_library(lien_image ${LIEN_IMAGE_SOURCES} ${LIEN_IMAGE_HEADERS})
target_link_libraries(lien_image lien_base stb)
target_include_directories(lien_image PUBLIC include)