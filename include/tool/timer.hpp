#pragma once

#include <chrono>

namespace shu {
namespace tool {
using namespace std::chrono;
class timer {
    decltype(high_resolution_clock::now()) t;

public:
    void start() {
        t = high_resolution_clock::now();
    }

    long stop() {
        return duration_cast<microseconds>(high_resolution_clock::now() - t).count();
    }
};
} // namespace tool
} // namespace shu