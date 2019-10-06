SET(LIEN_BASE_SOURCES
    src/assert.cpp)

add_library(lien_base ${LIEN_BASE_SOURCES})
target_include_directories(lien_base PUBLIC include)