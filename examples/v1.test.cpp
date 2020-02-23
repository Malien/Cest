#include <thread>

#include "../src/cest.hpp"

bool isEven(const int& a) {
    return a % 2 == 0;
}

struct E {};

int main() {
    test("should pass", []{
        expect(3).toBe(3);
    });
    test("should fail", []{
        expect(2).toBe(4);
    });
    test("should throw", []{
        throw std::out_of_range("unlucky");
    });
    test("should throw non error", []{
        throw "h";
    });
    test("should pass own condition", []{
        expect(2).toPass(isEven);
    });
    test("should fail own condition", []{
        expect(3).toPass(isEven);
    });
    testp("should fail after 1s", []{
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
        expect(5).toBe(6);
    });
    test("should take about 1s", []{
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    });
    testp("should be executed in parallel", []{
        expect(5).toBe(5);
    });
    test("should pass not to be the same", []{
        (not expect(5)).toBe(4);
    });
    test("should fail not to be the same", []{
        (not expect(5)).toBe(5);
    });
    test("shoud be close to", []{
        expect(4.9999999).toBeCloseTo(5);
    });
    test("struct", []{
        struct {} a;
        expect(a);
    });
    test("should intercept exception", []{
        expect([]{
            throw E{};
        }).toThrow<E>();
    });
    test("should not intercept exception", []{
        expect([]{}).toThrow<E>();
    });
    test("should intecept different STL exception", []{
        expect([]{
            throw std::range_error{"Wrong one"};
        }).toThrow<E>();
    });
    test("should intecept different STL exception", []{
        expect([]{
            throw 2;
        }).toThrow<E>();
    });
}