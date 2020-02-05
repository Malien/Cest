#pragma once
#include <string>
#include <functional>
#include <optional>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <cmath>
#include "colorize.hpp"
#include "message.hpp"

#define test(name, test_func) cest::sequentialTest(name, test_func, __FILE__, __LINE__)
#define testp(name, test_func) cest::parallelTest(name, test_func, __FILE__, __LINE__)
#define expect(value) cest::expectImpl(value, __FILE__, __LINE__)

namespace cest {

    namespace _global {
        std::mutex taskPoolMutex;
        std::mutex consoleMutex;
        std::vector<std::thread> taskPool;
    }

    enum class mode { sequential, parallel };

    struct TestFailure {
        std::string file;
        int line;
        std::optional<std::string> expected_repr;
        std::string result_repr;
        bool negated = false;
    };

    template <typename T> struct TestCase {
        const T& val;
        std::string file;
        int line;
        bool negated = false;

        void toBe(const T& val) const {
            if ((this->val == val) ^ !negated) {
                std::stringstream expected, result;
                expected << val;
                result << this->val;
                throw TestFailure{file, line, expected.str(), result.str(), negated};
            }
        }

        void toBeCloseTo(T val, T eps = 1e-4) const {
            if ((fabs(this->val - val) < eps) ^ !negated) {
                std::stringstream expected, result;
                expected << val;
                result << this->val;
                throw TestFailure{file, line, expected.str(), result.str(), negated};
            }
        }

        void toPass(const std::function<bool(const T&)>& func) const {
            if (func(val) ^ !negated) {
                std::stringstream result;
                result << val;
                throw TestFailure{file, line, std::nullopt, result.str(), negated};
            }
        }

        const TestCase<T> operator!() const {
            return { val, file, line, !negated };
        }
    };

    void f(std::ostream& os) {
        os << colorize::standart::foreground::cyan;
    }

    template <typename T> const TestCase<T> expectImpl(const T& val,
                                                       const char* filename = "Unknown",
                                                       int line = 0) { return { val, filename, line }; }

    void sequentialTest(const std::string_view& name, 
                        std::function<void()> test_func,
                        const char* filename = "Unknown",
                        int line = 0)
    {
        using namespace colorize::standart;
        auto start = std::chrono::high_resolution_clock::now();
        try {
            test_func();
            std::unique_lock lock(_global::consoleMutex);
            std::cout << message::pass {name, filename, line, start} << std::endl;
        } catch (const TestFailure& failure) {
            std::unique_lock lock(_global::consoleMutex);
            std::string expectedMsg = (failure.negated) ? "Expected NOT: " : "Expected";
            std::cerr << message::fail {name, filename, line, start} << std::endl
                      << "Test failed at " << foreground::brightBlack << failure.file << ':' << failure.line << colorize::end << std::endl;
            if (failure.expected_repr.has_value()) {
                std::cerr << foreground::cyan << "\t" << expectedMsg << "\n\t\t" << failure.expected_repr.value() << colorize::end << std::endl;
            }
            std::cerr << foreground::red << "\tGot: \n\t\t" << failure.result_repr << colorize::end << std::endl;
        } catch (std::exception e) {
            std::unique_lock lock(_global::consoleMutex);
            std::cerr << message::pass {name, filename, line, start} << std::endl
                      << "\tTest threw STL exception: " << foreground::brightRed << e.what() << colorize::end << std::endl;
        } catch (...) {
            std::unique_lock lock(_global::consoleMutex);
            std::cerr << message::fail {name, filename, line, start} << std::endl
                      << foreground::brightRed << "\tTest threw unknown exception" << colorize::end << std::endl;
        }
    }

    void parallelTest(const std::string_view& name,
                      std::function<void()> test_func,
                      const char* filename = "Unknown",
                      int line = 0)
    {
        std::unique_lock lock{_global::taskPoolMutex};
        _global::taskPool.push_back(std::thread([&]{
            sequentialTest(name, test_func, filename, line);
        }));
    }

    void joinParallelTests() {
        std::unique_lock lock{_global::taskPoolMutex};
        for (auto& task : _global::taskPool) {
            task.join();
        }
    }

}
