#pragma once

#ifdef NDEBUG
    #define LIEN_RELEASE_ONLY(keyword) keyword
    #define LIEN_DEBUG_ONLY(keyword)
#else
    #define LIEN_RELEASE_ONLY(keyword)
    #define LIEN_DEBUG_ONLY(keyword) keyword
#endif