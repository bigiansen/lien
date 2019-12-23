#pragma once

#ifdef NDEBUG
    #define XSTEG_RELEASE_CONSTEXPR constexpr
#else
    #define XSTEG_RELEASE_CONSTEXPR
#endif