set(LIEN_STRUTILS_TESTS_SOURCES
    src/main.cpp
    src/strutils_contains.cpp
    src/strutils_replace.cpp
    src/strutils_split.cpp
    src/strutils_toupper_tolower.cpp
    src/strutils_trim.cpp
)

add_executable(lien_strutils_tests ${LIEN_STRUTILS_TESTS_SOURCES})
target_link_libraries(lien_strutils_tests lien_strutils Catch2::Catch2)