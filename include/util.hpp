#pragma once

#include <array>
#include <cmath>
#include <vector>

#include "thread_pool.hpp"

namespace shu {
template <typename Func, typename T>
void parallel(thread_pool &pool, std::vector<T> const &vec, Func const &func);

template <typename Func, typename T, size_t SIZE>
void parallel(thread_pool &pool, std::array<T, SIZE> const &arr, Func const &func);

template <typename Func, typename Idx>
void parallel(thread_pool &pool, Idx beg, Idx end, Func const &func);

template <typename Func, typename T>
inline void parallel(thread_pool &pool, std::vector<T> const &vec, Func const &func) {
    parallel(pool, static_cast<size_t>(0), vec.size(), [&](size_t i) {
        func(i, vec[i]);
    });
}

template <typename Func, typename T, size_t SIZE>
inline void parallel(thread_pool &pool, std::array<T, SIZE> const &arr, Func const &func) {
    parallel(pool, static_cast<size_t>(0), arr.size(), [&](size_t i) {
        func(i, arr[i]);
    });
}

template <typename Func, typename Idx>
inline void parallel(thread_pool &pool, Idx beg, Idx end, Func const &func) {
    const double size = static_cast<double>(end - beg);
    const double tnum = static_cast<double>(pool.size());
    const Idx part = static_cast<Idx>(std::ceil(size / tnum));

    if (part == 0) {
        return;
    }

    std::function<void(Idx, Idx)> sub_func = [&](Idx local_beg, Idx local_end) {
        for (Idx i = local_beg; i < local_end; i += 1) {
            func(i);
        }
    };

    std::vector<future<void>> future_vec;
    future_vec.reserve(pool.size());

    for (size_t i = 0; i < pool.size(); i += 1) {
        const Idx local_beg = beg + part * i;
        const Idx local_end = std::min(end, beg + part * (static_cast<Idx>(i) + 1));
        future_vec.emplace_back(pool.execute(sub_func, local_beg, local_end));
    }

    // future_vec destructor will wait until all future is finished.
}

template <typename Op, typename Container, typename T>
T reduce(thread_pool &pool, Container const &vec, Op const &op, T const &init);

template <typename Trans, typename Op, typename Container, typename T>
T reduce(thread_pool &pool, Container const &vec, Trans const &trans, Op const &op, T const &init);

template <typename Op, typename Container, typename T>
inline T reduce(thread_pool &pool, Container const &vec, Op const &op, T const &init) {
    return reduce(
        pool, vec, [](T const &e) { return e; }, op, init);
}

template <typename Trans, typename Op, typename Container, typename T>
inline T reduce(thread_pool &pool, Container const &vec, Trans const &trans, Op const &op, T const &init) {
    const double size = static_cast<double>(vec.size());
    const double tnum = static_cast<double>(pool.size());
    const size_t part = static_cast<size_t>(std::ceil(size / tnum));

    if (part == 0) {
        return init;
    }

    std::function<T(size_t, size_t)> sub_func = [&](size_t local_beg, size_t local_end) {
        T val = init;
        for (size_t i = local_beg; i < local_end; i += 1) {
            val = op(val, trans(vec[i]));
        }
        return val;
    };

    std::vector<future<T>> future_vec;
    future_vec.reserve(pool.size());

    for (size_t i = 0; i < pool.size(); i += 1) {
        const size_t local_beg = part * i;
        const size_t local_end = std::min(vec.size(), part * (static_cast<size_t>(i) + 1));
        future_vec.emplace_back(pool.execute(sub_func, local_beg, local_end));
    }

    T val = init;
    for (size_t i = 0; i < pool.size(); i += 1) {
        val = op(val, future_vec[i]);
    }

    return val;
}
} // namespace shu