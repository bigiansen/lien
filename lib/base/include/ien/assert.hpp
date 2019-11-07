#pragma once

#include <stdexcept>
#include <string>

namespace ien
{
    class assertion_error : std::logic_error
    {
    public:
        assertion_error(const std::string& what)
            : std::logic_error(what)
        { }
        
        assertion_error(const char* what)
            : std::logic_error(what)
        { }
    };
    
    extern void debug_assert(bool cond, const std::string& msg);
    extern void runtime_assert(bool cond, const std::string& msg);
}

#define LIEN_NOT_IMPLEMENTED() runtime_assert(false, "Method not implemented!")