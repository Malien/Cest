#pragma once
#include <string>
#include <functional>
#include <optional>
#include <iostream>
#include <sstream>
#include "colorize.hpp"

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
        try {
            test_func();
            std::cout << foreground::brightGreen << "✓" << colorEnd << ' ' << name << ' ' 
                      << foreground::brightBlack << filename << ':' << line << colorEnd << std::endl;
        } catch (const TestFailure& failure) {
            std::cerr << foreground::brightRed << 'x' << colorEnd << ' ' << name << ' '
                      << foreground::brightBlack << filename << ':' << line << colorEnd << std::endl;
            std::cerr << "Test failed at " << foreground::brightBlack << failure.file << ':' << failure.line << colorEnd << std::endl;
            if (failure.expected_repr.has_value()) {
                std::cerr << foreground::cyan << "\tExpected: \n\t\t" << failure.expected_repr.value() << colorEnd << std::endl;
            }
            std::cerr << foreground::red << "\tGot: \n\t\t" << failure.result_repr << colorEnd << std::endl;
        } catch (std::exception e) {
            std::cerr << foreground::brightRed << 'x' << colorEnd << ' ' << name << ' '
                      << foreground::brightBlack << filename << ':' << line << colorEnd << std::endl;
            std::cerr << "\tTest threw STL expetion: " << foreground::brightRed << e.what() << colorEnd << std::endl;
        } catch (...) {
            std::cerr << foreground::brightRed << 'x' << colorEnd << ' ' << name << ' '
                      << foreground::brightBlack << filename << ':' << line << colorEnd << std::endl;
            std::cerr << foreground::brightRed << "\tTest threw unknown exeption" << colorEnd << std::endl;
        }
    }

}