set(STRUTILS_SOURCES
    src/strutils.cpp
)

set(STRUTILS_HEADERS
    include/ien/strutils.hpp
)

FILE(GLOB LIEN_STRUTILS_HEADERS	include/ien/*.hpp)

add_library(lien_strutils STATIC ${STRUTILS_SOURCES} ${LIEN_STRUTILS_HEADERS})
target_include_directories(lien_strutils PUBLIC include)
target_link_libraries(lien_strutils lien_base)