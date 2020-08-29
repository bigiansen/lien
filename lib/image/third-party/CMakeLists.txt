if(MSVC)
    add_definitions(/W0)
else()
    add_definitions(-w)
endif()

add_subdirectory(stb)