#pragma once

#ifdef NDEBUG
    #define LIEN_RELEASE_CONSTEXPR constexpr
#else
    #define LIEN_RELEASE_CONSTEXPR
#endif