#include <cassert>
#include <thread>

#include "semaphore.hpp"

namespace {
static size_t x = 0;
void run(shu::semaphore &sem) {
    for (int i = 0; i < 500; i += 1) {
        sem.decrement();
        x += 1;
        sem.increment();
    }
}
} // namespace

void test_semaphore() {
    shu::semaphore sem{1};

    assert(x == 0);

    std::thread t0{run, std::ref(sem)};
    std::thread t1{run, std::ref(sem)};
    std::thread t2{run, std::ref(sem)};
    std::thread t3{run, std::ref(sem)};
    std::thread t4{run, std::ref(sem)};
    std::thread t5{run, std::ref(sem)};
    std::thread t6{run, std::ref(sem)};
    std::thread t7{run, std::ref(sem)};
    std::thread t8{run, std::ref(sem)};
    std::thread t9{run, std::ref(sem)};

    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();

    assert(x == 5000);
}