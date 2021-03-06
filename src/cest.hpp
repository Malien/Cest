#pragma once
#include <string>
#include <functional>
#include <optional>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <cmath>
#include <typeinfo>
#include "colorize.hpp"
#include "message.hpp"

#ifdef __COUNTER__
#define UNIQUE __COUNTER__
#else
#define UNIQUE __LINE__
#endif

#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res

#define UNIQUE_NAME(base) PP_CAT(base, UNIQUE)

#define test(name, test_func) cest::sequentialTest(name, test_func, __FILE__, __LINE__)
#define testp(name, test_func) cest::ParallelTestHandler UNIQUE_NAME(__parallel_test_) = cest::parallelTest(name, test_func, __FILE__, __LINE__)
#define expect(value) cest::expectImpl(value, __FILE__, __LINE__)

namespace cest {

    class internal {
        static inline std::mutex consoleMutex;
        static inline int exitCode = EXIT_SUCCESS;
        friend void sequentialTest(const std::string_view&, std::function<void()>, const char*, int);
        friend int exitCode();
    };

    int exitCode() { return internal::exitCode; }

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

        template<class Cmp = std::equal_to<T>> void toBe(const T& val, Cmp comparator = Cmp{}) const {
            if (comparator(this->val, val) ^ !negated) {
                failWithExpected(val);
            }
        }

        template<typename I> 
        typename std::enable_if<std::is_convertible<T, I>::value && std::is_floating_point<I>::value>::type 
        toBeCloseTo(I val, I eps = 1e-4) const {
            if (((this->val == val) || (abs(this->val - val) < this->val * eps)) ^ !negated) {
                failWithExpected(val);
            }
        }

        void toPass(const std::function<bool(const T&)>& func) const {
            if (func(val) ^ !negated) {
                fail(val);
            }
        }

        template<typename U> void toPass(const std::function<bool(const T&, const U&)>& func, const U& param) const {
            if (func(val, param) ^ !negated) {
                fail();
            }
        }

        // TODO: get rid of duck typing and provide template constraints
        template<typename U> void toThrow() const {
            std::string thrownName = typeid(U).name();
            try {
                val();
                throw Rethrow{};
            } catch(U e) {
                if (negated) throw TestFailure{file, line, "To throw " + thrownName, thrownName, negated};
            } catch(Rethrow e) {
                if (!negated) throw TestFailure{file, line, "To throw " + thrownName, "Nothing", negated};
            } catch (std::exception e) {
                throw TestFailure{file, line, "To throw " + thrownName, e.what(), negated};
            } catch(...) {
                throw TestFailure{file, line, "To throw " + thrownName, "Non STL exception", negated};
            }
        }

        const TestCase<T> operator!() const {
            return { val, file, line, !negated };
        }

        private: 
            void fail(T val) const {
                std::stringstream result;
                result << val;
                throw TestFailure{file, line, std::nullopt, result.str(), negated};
            }

            void failWithExpected(T val) const {
                std::stringstream expectedStream, resultStream;
                expectedStream << val;
                resultStream << this->val;
                throw TestFailure{file, line, expectedStream.str(), resultStream.str(), negated};
            }

            struct Rethrow {};
    };

    template <typename T> const TestCase<T> expectImpl(const T& val,
                                                       const char* filename = "Unknown",
                                                       int line = 0) { return { val, filename, line }; }

    inline void sequentialTest(const std::string_view& name, 
                        std::function<void()> test_func,
                        const char* filename = "Unknown",
                        int line = 0)
    {
        using namespace colorize::standart;
        auto start = std::chrono::high_resolution_clock::now();
        try {
            test_func();
            std::unique_lock lock(internal::consoleMutex);
            std::cout << message::pass {name, filename, line, start} << std::endl;
        } catch (const TestFailure& failure) {
            std::unique_lock lock(internal::consoleMutex);
            std::string expectedMsg = (failure.negated) ? "Expected NOT: " : "Expected: ";
            std::cerr << message::fail {name, filename, line, start} << std::endl
                      << "Test failed at " << foreground::brightBlack << failure.file << ':' << failure.line << colorize::end << std::endl;
            if (failure.expected_repr.has_value()) {
                std::cerr << foreground::cyan << "\t" << expectedMsg << "\n\t\t" << failure.expected_repr.value() << colorize::end << std::endl;
            }
            std::cerr << foreground::red << "\tGot: \n\t\t" << failure.result_repr << colorize::end << std::endl;
            internal::exitCode = EXIT_FAILURE;
        } catch (std::exception e) {
            std::unique_lock lock(internal::consoleMutex);
            std::cerr << message::fail {name, filename, line, start} << std::endl
                      << "\tTest threw STL exception: " << foreground::brightRed << e.what() << colorize::end << std::endl;
            internal::exitCode = EXIT_FAILURE;
        } catch (...) {
            std::unique_lock lock(internal::consoleMutex);
            std::cerr << message::fail {name, filename, line, start} << std::endl
                      << foreground::brightRed << "\tTest threw unknown exception" << colorize::end << std::endl;
            internal::exitCode = EXIT_FAILURE;
        }
    }

    struct ParallelTestHandler {
        std::thread thread;
        ~ParallelTestHandler() {
            thread.join();
        }
    };

    inline ParallelTestHandler parallelTest(const std::string_view& name,
                                     std::function<void()> test_func,
                                     const char* filename = "Unknown",
                                     int line = 0)
    {
        return {std::thread([=]{
            sequentialTest(name, test_func, filename, line);
        })};
    }

}
