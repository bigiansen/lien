#pragma once

#include <ien/type_traits.hpp>

#include <functional>
#include <thread>
#include <vector>

namespace ien
{
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // PARALLEL FOR
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    struct parallel_for_params
    {
        long offset = 0;
        long count = 0;
        long stride = 1;
        unsigned int max_threads = std::thread::hardware_concurrency();

        parallel_for_params() noexcept { }
        parallel_for_params(long vcount) noexcept : count(vcount) { }
    };

    using parallel_for_pred_t = std::function<void(long)>;
    extern void parallel_for(parallel_for_params desc, parallel_for_pred_t pred);
}