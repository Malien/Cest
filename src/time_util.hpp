#pragma once
#include <chrono>
#include <iostream>

namespace time_util{

    struct TimeDiff {
        double seconds;
        double milliseconds() const {
            return seconds * 1000;
        }
    };

    TimeDiff timeDiff(std::chrono::time_point<std::chrono::steady_clock> start) {
        using namespace std::chrono;
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        return {diff.count()};
    }

}

std::ostream& operator<<(std::ostream& os, const time_util::TimeDiff& diff) {
    if (diff.seconds < 1) {
        if (diff.milliseconds() < 1) {
            return os << "<1ms";
        }
        return os << diff.milliseconds() << "ms";
    }
    return os << diff.seconds << "s";
}