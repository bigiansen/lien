#include <ien/assert.hpp>
#include <iostream>

namespace ien
{
    void debug_assert(bool cond, const std::string& msg)
    {
    #ifdef NDEBUG
        return;
    #else
        if(!cond)
        {
            std::cerr << msg << std::endl;
            throw assertion_error(msg);
        }
    #endif
    }

    void runtime_assert(bool cond, const std::string& msg)
    {
        if(!cond)
        {
            std::cerr << msg << std::endl;
            throw assertion_error(msg);
        }
    }
}