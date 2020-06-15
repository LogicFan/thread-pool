#include <atomic>
#include <cassert>
#include <chrono>
#include <thread>

#include <iostream>

#include "thread_pool.hpp"

namespace {
static std::atomic<int> x = 0;

void test1() {
    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    x += 1;
}

void test2(int y) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    x += y;
}

int test3(int y, int z) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    return y + z;
}

void test4(int x) {
    throw std::runtime_error{"runtime_error"};
}

int test5(int x) {
    throw std::runtime_error{"runtime_error"};
}

} // namespace

void test_thread_pool() {
    shu::thread_pool threads{100, 10};
    std::vector<shu::future<void>> futures_void;
    std::vector<shu::future<int>> futures_int;

    // test for void()
    x = 0;

    for (int i = 0; i < 10000; i += 1) {
        auto future = threads.execute(std::function<decltype(test1)>{test1});
        futures_void.push_back(std::move(future));
    }

    futures_void.clear();
    assert(x == 10000);

    // test for void(args)
    x = 0;

    for (int i = 0; i < 10000; i += 1) {
        auto future = threads.execute(std::function<decltype(test2)>{test2}, i);
        futures_void.push_back(std::move(future));
    }

    futures_void.clear();
    assert(x == 49995000);

    // test for int(args)
    for (int i = 0; i < 100; i += 1) {
        auto future = threads.execute(std::function<decltype(test3)>{test3}, i, i + 1);
        futures_int.push_back(std::move(future));
    }

    for (int i = 0; i < 100; i += 1) {
        assert(futures_int[i] == 2 * i + 1);
    }

    {
        auto future = threads.execute(std::function<decltype(test4)>{test4}, 1);
        try {
            future.wait();
            assert(false);
        } catch (std::runtime_error &) {
        }
    }

    {
        auto future = threads.execute(std::function<decltype(test5)>{test5}, 1);
        try {
            future.wait();
            assert(false);
        } catch (std::runtime_error &) {
        }
    }

    {
        auto future = threads.execute(std::function<decltype(test5)>{test5}, 1);
        try {
            int t = future;
            (void) t;
            assert(false);
        } catch (std::runtime_error &) {
        }
    }

    assert(threads.size() == 100);
}