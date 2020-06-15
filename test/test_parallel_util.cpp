#include <cassert>
#include <random>
#include <vector>

#include "util.hpp"

void test_parallel_util() {
    std::vector<int> a_vec;
    std::vector<int> b_vec;
    std::vector<int> c_vec;
    std::vector<int> d_vec;
    std::random_device random;

    shu::thread_pool pool{};

    for (size_t i = 0; i < 10000; i += 1) {
        a_vec.push_back(static_cast<int>(random()));
        b_vec.push_back(static_cast<int>(random()));
        c_vec.push_back(0);
        d_vec.push_back(0);
    }

    shu::parallel(pool, 233, 6666, [&](size_t i) {
        c_vec[i] = a_vec[i] + b_vec[i];
    });

    for (size_t i = 0; i < 233; i += 1) {
        assert(c_vec[i] == 0);
    }

    for (size_t i = 233; i < 6666; i += 1) {
        assert(c_vec[i] == a_vec[i] + b_vec[i]);
    }

    for (size_t i = 6666; i < 10000; i += 1) {
        assert(c_vec[i] == 0);
    }

    shu::parallel(pool, c_vec, [&](size_t idx, int ele) {
        d_vec[idx] = ele * 2;
    });

    for (size_t i = 0; i < 10000; i += 1) {
        assert(d_vec[i] == c_vec[i] * 2);
    }

    int result_sum = shu::reduce(
        pool, a_vec, [](int x, int y) {
            return x + y;
        },
        0);

    int expect_sum = 0;
    for (size_t i = 0; i < 10000; i += 1) {
        expect_sum += a_vec[i];
    }

    assert(result_sum == expect_sum);

    std::vector<int> empty;

    shu::parallel(pool, empty, [](size_t idx, int ele) {
        (void)idx;
        (void)ele;
    });

    int init = shu::reduce(
        pool, empty, [](int x, int y) { return x * y; }, 1);
    assert(init == 1);
}