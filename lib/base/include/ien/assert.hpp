#pragma once

#include <ien/debug.hpp>
#include <stdexcept>
#include <string>

namespace ien
{
    XSTEG_RELEASE_CONSTEXPR inline void debug_assert(bool cond, const std::string& msg)
    {
    #ifdef NDEBUG
        return;
    #else
        if(!cond) { throw std::logic_error(msg); }
    #endif
    }

    inline void runtime_assert(bool cond, const std::string& msg)
    {
        if(!cond) { throw std::logic_error(msg); }
    }
}

#define LIEN_NOT_IMPLEMENTED() runtime_assert(false, "Method not implemented!")