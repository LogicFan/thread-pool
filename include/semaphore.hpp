#pragma once

#include <condition_variable>
#include <mutex>

namespace shu {
class semaphore final {
private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    size_t m_count;

public:
    semaphore(size_t count = 0);
    void increment();
    void decrement();
};
} // namespace shu
