set(STB_SOURCES
    src/stb_image.impl.cpp
    src/stb_image_resize.impl.cpp
    src/stb_image_write.impl.cpp)
    
set(STB_HEADERS
    include/*.h
    include/*.hpp
)

add_library(stb ${STB_SOURCES})
target_include_directories(stb PUBLIC include)
target_link_libraries(stb lien_base)