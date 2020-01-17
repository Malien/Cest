#include "../src/v1.hpp"

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
}