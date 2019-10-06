#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

#include <ien/type_traits.hpp>

namespace ien
{
    typedef std::function<void()> task_t;
    class task_queue
    {
    private:
        std::vector<task_t> _tasks;
        std::atomic<int> _current_task = 0;
        std::vector<std::thread> _threads;
        std::mutex _run_mux;
        size_t _max_concurrent;
        bool _detached = false;
        bool _started = false;

    public:
        task_queue(size_t max_concurrent_tasks = std::thread::hardware_concurrency()) noexcept;

        template<typename TTask, typename = tt::enable_if_is_type<TTask, task_t>>
        void emplace_back(TTask&& task)
        {
            _tasks.push_back(std::forward(task));
        }

        template<typename TContainer, 
            typename = std::enable_if_t<tt::is_iterable_of<TContainer, task_t>>>
        void emplace_back_many(TContainer&& cont)
        {
            if constexpr(std::is_rvalue_reference_v<decltype(cont)>)
            {
                std::move(cont.begin(), cont.end(), std::back_inserter(_tasks));
            }
            else
            {
                std::copy(cont.begin(), cont.end(), std::back_inserter(_tasks));
            }
        }

        void run(bool detached = false);
        void join();

    private:
        const task_t* dequeue_task();
        void worker_thread();
        void join_threads();
    };
}