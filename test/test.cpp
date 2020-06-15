#include <iostream>

#include "tool/timer.hpp"

#define TESTINIT \
    shu::tool::timer t {}

#define DECLARE(name) \
    void test##_##name()

#define TEST(name)                                               \
    std::cerr << "[ test - " << #name << " ] : start." << std::endl; \
    t.start();                                                   \
    test##_##name();                                             \
    std::cerr << "[ test - " << #name << " ] : pass (" << t.stop() << " us)." << std::endl;

DECLARE(blocking_queue);
DECLARE(semaphore);
DECLARE(future);
DECLARE(thread_pool);
DECLARE(parallel_util);

int main(int argc, char *argv[]) {
    TESTINIT;

    TEST(blocking_queue);
    TEST(semaphore);
    TEST(future);
    TEST(thread_pool);
    TEST(parallel_util);

    return 0;
}