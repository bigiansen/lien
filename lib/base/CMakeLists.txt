SET(LIEN_BASE_SOURCES
    src/bit_view.cpp
	src/platform.cpp)

FILE(GLOB LIEN_BASE_HEADERS include/ien/*.hpp)

add_library(lien_base ${LIEN_BASE_SOURCES} ${LIEN_BASE_HEADERS})
target_include_directories(lien_base PUBLIC include)