#pragma once

#if __has_include(<filesystem>)
    #include <filesystem>
    #define LIEN_FS std::filesystem

#elif __has_include(<experimental/filesystem>)
    #ifndef LIEN_FS_EXPERIMENTAL
        #define LIEN_FS_EXPERIMENTAL
        #warning "Using <experimental/filesystem>"
        #include <experimental/filesystem>
        #define LIEN_FS std::experimental::filesystem
    #endif
#else
    #error "<filesytem> header was not found!"
#endif