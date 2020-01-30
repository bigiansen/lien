if(NOT TARGET lien_base)
    add_subdirectory(base)
endif()

if(LIEN_BUILD_STRUTILS AND NOT TARGET lien_strutils)
    add_subdirectory(strutils)
endif()

if(LIEN_BUILD_PARALLEL AND NOT TARGET lien_parallel)
    add_subdirectory(parallel)
endif()

if(LIEN_BUILD_IMAGE AND NOT TARGET lien_image)
    add_subdirectory(image)
endif()