#pragma once

#include <atomic>
#include <exception>
#include <iostream>
#include <memory>

#include "semaphore.hpp"

void test_future();
class thread_pool;

namespace shu {
template <typename T>
class future {
private:
    std::unique_ptr<semaphore> m_sem;
    std::unique_ptr<std::exception_ptr> m_exception;
    std::unique_ptr<T> m_result;

    future(semaphore *&sem, std::exception_ptr *&ecp, T *&res);

    friend void ::test_future();
    friend class thread_pool;

public:
    future(future &&rhs) = default;
    ~future();

    future &operator=(future &&rhs) = default;
    operator T();

    void wait();
};

template <typename T>
inline future<T>::future(semaphore *&sem, std::exception_ptr *&ecp, T *&res)
    : m_sem{std::make_unique<semaphore>(0)}, m_exception{std::make_unique<std::exception_ptr>()},
      m_result{std::make_unique<T>()} {
    sem = m_sem.get();
    ecp = m_exception.get();
    res = m_result.get();
}

template <typename T>
inline future<T>::~future() {
    if (m_sem) {
        m_sem->decrement();
    }
}

template <typename T>
inline future<T>::operator T() {
    if (m_sem) {
        m_sem->decrement();
        if (*m_exception) {
            m_sem->increment();
            std::rethrow_exception(*m_exception);
        }
        auto res = *m_result;
        m_sem->increment();
        return res;
    }
    throw std::runtime_error{"invalid semaphore."};
}

template <typename T>
inline void future<T>::wait() {
    if (m_sem) {
        m_sem->decrement();
        if (*m_exception) {
            m_sem->increment();
            std::rethrow_exception(*m_exception);
        }
        m_sem->increment();
    }
    throw std::runtime_error{"invalid semaphore."};
}

template <>
class future<void> {
private:
    std::unique_ptr<semaphore> m_sem;
    std::unique_ptr<std::exception_ptr> m_exception;

    future(semaphore *&sem, std::exception_ptr *&ecp);

    friend void ::test_future();
    friend class thread_pool;

public:
    future(future &&rhs) = default;
    ~future();

    future &operator=(future &&rhs) = default;

    void wait();
};

inline future<void>::future(semaphore *&sem, std::exception_ptr *&ecp)
    : m_sem{std::make_unique<semaphore>(0)}, m_exception{std::make_unique<std::exception_ptr>()} {
    sem = m_sem.get();
    ecp = m_exception.get();
}

inline future<void>::~future() {
    if (m_sem) {
        m_sem->decrement();
    }
}

inline void future<void>::wait() {
    if (m_sem) {
        m_sem->decrement();
        if (*m_exception) {
            m_sem->increment();
            std::rethrow_exception(*m_exception);
        }
        m_sem->increment();
    }
    throw std::runtime_error{"invalid semaphore."};
}
} // namespace shu
