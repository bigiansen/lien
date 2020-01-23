#pragma once

#ifdef NDEBUG
    #define LIEN_RELEASE_CONSTEXPR constexpr
	#define LIEN_RELEASE_NOEXCEPT noexcept
#else
    #define LIEN_RELEASE_CONSTEXPR
	#define LIEN_RELEASE_NOEXCEPT
#endif