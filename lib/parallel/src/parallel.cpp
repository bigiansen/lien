#include <ien/parallel.hpp>
#include <stdexcept>

namespace ien
{
    void parallel_for_worker(long offset, long count, long stride, parallel_for_pred_t pred)
    {
        long current_count = 0;
        while (current_count < count)
        {
            long idx = offset + (current_count * stride);
            pred(idx);
            ++current_count;
        }
    }

    void parallel_for(parallel_for_params params, parallel_for_pred_t pred, bool detached)
    {
        if (params.count == 0) { return; }
        if (params.max_threads == 0)
        {
            throw std::invalid_argument("Invalid max threads count!");
        }

        std::vector<std::thread> threads;
        threads.reserve(params.max_threads);

        const int segment_size = params.count / params.max_threads;

        for (unsigned int i = 0; i < params.max_threads - 1; ++i)
        {
            int offset = i * segment_size;
            threads.push_back(
                std::thread(&parallel_for_worker, offset, segment_size, params.stride, pred)
            );
        }

        int last_offset = (params.max_threads - 1) * segment_size;
        int last_segsz = segment_size + (params.count % params.max_threads);
        threads.push_back(
            std::thread(&parallel_for_worker, last_offset, last_segsz, params.stride, pred)
        );

        if (detached) {
            for (unsigned int i = 0; i < params.max_threads; ++i) {
                threads[i].detach();
            }
        }
        else {
            for (unsigned int i = 0; i < params.max_threads; ++i) {
                threads[i].join();
            }
        }
    }
}