#pragma once
#include <string>
#include <functional>
#include <optional>
#include <iostream>
#include <sstream>
#include "colorize.hpp"
#include "time_util.hpp"

#define test(name, test_func) cest::testImpl(name, test_func, __FILE__, __LINE__)
#define expect(value) cest::expectImpl(value, __FILE__, __LINE__)

namespace cest {

    struct TestFailure {
        std::string file;
        int line;
        std::optional<std::string> expected_repr;
        std::string result_repr;
    };

    template <typename T> struct TestCase {
        const T& val;
        std::string file;
        int line;

        void toBe(const T& val) const {
            if (this->val != val) {
                std::stringstream expected, result;
                expected << val;
                result << this->val;
                throw TestFailure{file, line, expected.str(), result.str()};
            }
        }

        void toPass(const std::function<bool(const T&)>& func) const {
            if (!func(val)) {
                std::stringstream result;
                result << val;
                throw TestFailure{file, line, std::nullopt, result.str()};
            }
        }
    };

    template <typename T> const TestCase<T> expectImpl(const T& val, const char* filename, int line) {
        return {val, filename, line};
    }

    void testImpl(const std::string_view& name, std::function<void()> test_func, const char* filename, int line) {
        using namespace colorize::standart;
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
        try {
            test_func();
            std::cout << foreground::brightGreen << "✓" << colorize::end << ' ' << name << ' ' 
                      << foreground::brightBlack << filename << ':' << line << colorize::end 
                      << foreground::yellow << " (" << time_util::timeDiff(start) << ')' << colorize::end << std::endl;
        } catch (const TestFailure& failure) {
            std::cerr << foreground::brightRed << 'x' << colorize::end << ' ' << name << ' '
                      << foreground::brightBlack << filename << ':' << line << colorize::end
                      << foreground::yellow << " (" << time_util::timeDiff(start) << ')' << colorize::end << std::endl;
            std::cerr << "Test failed at " << foreground::brightBlack << failure.file << ':' << failure.line << colorize::end << std::endl;
            if (failure.expected_repr.has_value()) {
                std::cerr << foreground::cyan << "\tExpected: \n\t\t" << failure.expected_repr.value() << colorize::end << std::endl;
            }
            std::cerr << foreground::red << "\tGot: \n\t\t" << failure.result_repr << colorize::end << std::endl;
        } catch (std::exception e) {
            std::cerr << foreground::brightRed << 'x' << colorize::end << ' ' << name << ' '
                      << foreground::brightBlack << filename << ':' << line << colorize::end
                      << foreground::yellow << " (" << time_util::timeDiff(start) << ')' << colorize::end << std::endl;
            std::cerr << "\tTest threw STL exception: " << foreground::brightRed << e.what() << colorize::end << std::endl;
        } catch (...) {
            std::cerr << foreground::brightRed << 'x' << colorize::end << ' ' << name << ' '
                      << foreground::brightBlack << filename << ':' << line << colorize::end
                      << foreground::yellow << " (" << time_util::timeDiff(start) << ')' << colorize::end << std::endl;
            std::cerr << foreground::brightRed << "\tTest threw unknown exception" << colorize::end << std::endl;
        }
    }

}