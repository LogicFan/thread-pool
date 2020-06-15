#include <array>
#include <cassert>
#include <chrono>
#include <random>
#include <thread>

#include "blocking_queue.hpp"

namespace {
void produce(shu::blocking_queue<int> &queue, std::array<int, 1000> const &sample) {
    for (auto const &num : sample) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        queue.push(num);
    }
}

void consume(shu::blocking_queue<int> &queue, std::array<int, 1000> const &sample) {
    for (auto const &num : sample) {
        assert(queue.pop() == num);
    }
}
} // namespace

void test_blocking_queue() {
    shu::blocking_queue<int> queue{10};
    std::array<int, 1000> sample;

    std::random_device random;

    for (int i = 0; i < 1000; i += 1) {
        sample[i] = static_cast<int>(random());
    }

    std::thread producer{produce, std::ref(queue), std::ref(sample)};
    std::thread consumer{consume, std::ref(queue), std::ref(sample)};

    producer.join();
    consumer.join();
}
