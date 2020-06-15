#include <cassert>
#include <iostream>

#include "semaphore.hpp"

#define NDEBUG

namespace shu {
semaphore::semaphore(size_t count)
    : m_count{count} {}

void semaphore::increment() {
    std::lock_guard lock{m_mutex};
    m_count += 1;
    m_cv.notify_one();
}

void semaphore::decrement() {
    std::unique_lock lock{m_mutex};
    while (m_count == 0) {
        m_cv.wait(lock);
    }
    assert(m_count > 0);
    m_count -= 1;
}

} // namespace shu