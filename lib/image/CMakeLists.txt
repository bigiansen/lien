add_subdirectory(third-party)

SET(LIEN_IMAGE_SOURCES
    src/image.cpp
    src/pixel_trait.cpp)

add_library(lien_image ${LIEN_IMAGE_SOURCES})
target_link_libraries(lien_image lien_base stb)
target_include_directories(lien_image PUBLIC include)