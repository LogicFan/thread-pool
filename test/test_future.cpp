#include <array>
#include <cassert>
#include <chrono>
#include <random>
#include <thread>
#include <vector>

#include "future.hpp"

namespace {

static std::atomic<int> x = 0;

void set_int(shu::semaphore *sem, int *targ, int val) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    *targ = val;
    sem->increment();
}

void inc_int(shu::semaphore *sem) {
    x += 1;
    sem->increment();
}

} // namespace

void test_future() {
    std::array<int, 1000> sample;
    std::vector<shu::future<int>> futures_int;
    std::array<std::thread, 1000> threads;

    std::random_device random;

    assert(x == 0);

    for (int i = 0; i < 1000; i += 1) {
        sample[i] = static_cast<int>(random());
    }

    for (int i = 0; i < 1000; i += 1) {
        shu::semaphore *sem;
        int *targ;
        std::exception_ptr *ecp;
        futures_int.push_back(shu::future{sem, ecp, targ});
        threads[i] = std::thread{set_int, sem, targ, sample[i]};
    }

    for (int i = 0; i < 1000; i += 1) {
        assert(sample[i] == static_cast<int>(futures_int[i]));
    }

    for (int i = 0; i < 1000; i += 1) {
        threads[i].join();
    }

    std::vector<shu::future<void>> futures_void;
    for (int i = 0; i < 1000; i += 1) {
        shu::semaphore *sem;
        std::exception_ptr *ecp;
        futures_void.push_back(shu::future<void>{sem, ecp});
        threads[i] = std::thread{inc_int, sem};
    }

    futures_void.clear();
    assert(x == 1000);

    for (int i = 0; i < 1000; i += 1) {
        threads[i].join();
    }
}