find_package(Threads REQUIRED)

SET(LIEN_PARALLEL_SOURCES
    src/parallel.cpp
    src/task_queue.cpp
)

add_library(lien_parallel ${LIEN_PARALLEL_SOURCES})
target_link_libraries(lien_parallel lien_base Threads::Threads)
target_include_directories(lien_parallel PUBLIC include)