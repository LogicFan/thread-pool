#include <iostream>

#include "thread_pool.hpp"

namespace shu {
thread_pool::thread_pool(size_t thread_num, size_t queue_cap)
    : m_queue{queue_cap} {
    m_threads.reserve(thread_num);
    for (size_t i = 0; i < thread_num; i += 1) {
        m_threads.emplace_back(&thread_pool::executor, this);
    }
}

size_t thread_pool::size() {
    return m_threads.size();
}

thread_pool::~thread_pool() {
    std::function<bool()> terminator = []() { return false; };
    for (size_t i = 0; i < m_threads.size(); i += 1) {
        m_queue.push(terminator);
    }

    for (size_t i = 0; i < m_threads.size(); i += 1) {
        m_threads[i].join();
    }
}

void thread_pool::executor() {
    while (m_queue.pop()())
        ;
}
} // namespace shu