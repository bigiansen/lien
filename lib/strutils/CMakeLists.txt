set(STRUTILS_SOURCES
    src/strutils.cpp
)

set(STRUTILS_HEADERS
    include/ien/strutils.hpp
)

add_library(lien_strutils STATIC ${STRUTILS_SOURCES})
target_include_directories(lien_strutils PUBLIC include)
target_link_libraries(lien_strutils lien_base)