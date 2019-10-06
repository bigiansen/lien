add_subdirectory(base)
add_subdirectory(strutils)

if(LIEN_PARALLEL)
    add_subdirectory(parallel)
endif()

if(LIEN_IMAGE)
    add_subdirectory(image)
endif()