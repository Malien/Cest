#include "../src/cest.hpp"

bool isEven(const int& a) {
    return a % 2 == 0;
}

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
}