#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

#include "blocking_queue.hpp"
#include "future.hpp"
#include "semaphore.h"

namespace shu {
class thread_pool final {
private:
    std::vector<std::thread> m_threads;
    blocking_queue<std::function<bool()>> m_queue;

    void executor();

public:
    thread_pool(size_t thread_num = std::thread::hardware_concurrency(),
                size_t queue_size = std::thread::hardware_concurrency());
    ~thread_pool();

    size_t size();

    template <typename RETURN, typename... Args>
    future<RETURN> execute(std::function<RETURN(Args...)> const &func, Args... args);
};

template <typename RETURN, typename... Args>
future<RETURN> thread_pool::execute(std::function<RETURN(Args...)> const &func, Args... args) {
    if constexpr (std::is_void_v<RETURN>) {
        semaphore *sem;
        std::exception_ptr *ecp;

        future<RETURN> fut{sem, ecp};

        std::function<bool()> warpper = [args = std::make_tuple(std::forward<Args>(args)...),
                                         func, sem, ecp]() {
            try {
                std::apply(func, std::move(args));
            } catch (...) {
                *ecp = std::current_exception();
            }
            sem->increment();
            return true;
        };
        m_queue.push(std::move(warpper));
        return fut;
    } else {
        semaphore *sem;
        RETURN *res;
        std::exception_ptr *ecp;

        future<RETURN> fut{sem, ecp, res};

        std::function<bool()> warpper = [args = std::make_tuple(std::forward<Args>(args)...),
                                         func, sem, ecp, res]() {
            try {
                *res = std::apply(func, std::move(args));
            } catch (...) {
                *ecp = std::current_exception();
            }
            sem->increment();
            return true;
        };
        m_queue.push(std::move(warpper));
        return fut;
    }
}
} // namespace shu
