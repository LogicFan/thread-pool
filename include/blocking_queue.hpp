#pragma once

#include <cassert>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <vector>

namespace shu {
template <typename T>
class blocking_queue final {
private:
    std::mutex m_mutex;
    std::condition_variable m_empty;
    std::condition_variable m_full;
    std::vector<T> m_vector;

    const size_t m_capacity;

    size_t m_size;
    size_t m_first;
    size_t m_last;

public:
    blocking_queue(size_t capacity);
    void push(T const &rhs);
    void push(T &&rhs);
    T pop();
};

template <typename T>
inline blocking_queue<T>::blocking_queue(size_t capacity)
    : m_vector(capacity), m_capacity{capacity}, m_size{0}, m_first{0}, m_last{0} {}

template <typename T>
inline void blocking_queue<T>::push(T const &rhs) {
    std::unique_lock lock{m_mutex};
    while (m_size == m_capacity) {
        m_full.wait(lock);
    }
    assert(m_size < m_capacity);

    m_vector[m_last] = rhs;
    m_last = (m_last + 1) % m_capacity;
    m_size += 1;

    m_empty.notify_one();
}

template <typename T>
inline void blocking_queue<T>::push(T &&rhs) {
    std::unique_lock lock{m_mutex};
    while (m_size == m_capacity) {
        m_full.wait(lock);
    }
    assert(m_size < m_capacity);

    m_vector[m_last] = std::move(rhs);
    m_last = (m_last + 1) % m_capacity;
    m_size += 1;

    m_empty.notify_one();
}

template <typename T>
inline T blocking_queue<T>::pop() {
    std::unique_lock lock{m_mutex};
    while (m_size == 0) {
        m_empty.wait(lock);
    }
    assert(m_size > 0);

    T clone = std::move(m_vector[m_first]);
    m_first = (m_first + 1) % m_capacity;
    m_size -= 1;

    m_full.notify_one();

    return clone;
}
} // namespace shu
