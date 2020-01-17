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

    struct TestMessage {
        const std::string_view& name;
        const char* filename;
        int line;
        std::chrono::time_point<std::chrono::steady_clock> start;
    };

    struct PassMessage: public TestMessage {};
    struct FailMessage: public TestMessage {};

    void f(std::ostream& os) {
        os << colorize::standart::foreground::cyan;
    }

    template <typename T> const TestCase<T> expectImpl(const T& val, const char* filename, int line) {
        return {val, filename, line};
    }

    void testImpl(const std::string_view& name, std::function<void()> test_func, const char* filename, int line) {
        using namespace colorize::standart;
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
        try {
            test_func();
            std::cout << PassMessage {name, filename, line, start} << std::endl;
        } catch (const TestFailure& failure) {
            std::cerr << FailMessage {name, filename, line, start} << std::endl
                      << "Test failed at " << foreground::brightBlack << failure.file << ':' << failure.line << colorize::end << std::endl;
            if (failure.expected_repr.has_value()) {
                std::cerr << foreground::cyan << "\tExpected: \n\t\t" << failure.expected_repr.value() << colorize::end << std::endl;
            }
            std::cerr << foreground::red << "\tGot: \n\t\t" << failure.result_repr << colorize::end << std::endl;
        } catch (std::exception e) {
            std::cerr << FailMessage {name, filename, line, start} << std::endl
                      << "\tTest threw STL exception: " << foreground::brightRed << e.what() << colorize::end << std::endl;
        } catch (...) {
            std::cerr << FailMessage {name, filename, line, start} << std::endl
                      << foreground::brightRed << "\tTest threw unknown exception" << colorize::end << std::endl;
        }
    }

}

std::ostream& operator<< (std::ostream& os, const cest::PassMessage& msg) {
    using namespace colorize::standart;
    return os << foreground::brightGreen << "✓" << colorize::end << ' ' << msg.name << ' ' 
                << foreground::brightBlack << msg.filename << ':' << msg.line << colorize::end 
                << foreground::yellow << " (" << time_util::timeDiff(msg.start) << ')' << colorize::end;
}

std::ostream& operator<<(std::ostream& os, const cest::FailMessage& msg) {
    using namespace colorize::standart;
    return os << foreground::brightRed << 'x' << colorize::end << ' ' << msg.name << ' '
                << foreground::brightBlack << msg.filename << ':' << msg.line << colorize::end
                << foreground::yellow << " (" << time_util::timeDiff(msg.start) << ')' << colorize::end; 
}