#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

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

        template<typename TTask>
        void emplace_back(TTask&& task)
        {
            static_assert(
                std::is_same_v<std::decay_t<TTask>, task_t>, 
                "Not a valid task type"
            );
            _tasks.push_back(std::forward(task));
        }

        void run(bool detached = false);
        void join();

    private:
        const task_t* dequeue_task();
        void worker_thread();
        void join_threads();
    };
}