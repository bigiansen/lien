add_subdirectory(base)

if(LIEN_BUILD_STRUTILS)
    add_subdirectory(strutils)
endif()

if(LIEN_BUILD_PARALLEL)
    add_subdirectory(parallel)
endif()

if(LIEN_BUILD_IMAGE)
    add_subdirectory(image)
endif()