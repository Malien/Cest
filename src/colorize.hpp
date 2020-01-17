#pragma once
#include <iostream>

namespace colorize {
    struct ColorEndType {};
    const ColorEndType end;

    namespace standart {
        enum class foreground: unsigned char {
            black = 30,
            red = 31,
            green = 32,
            yellow = 33,
            blue = 34,
            magenta = 35,
            cyan = 36,
            white = 37,
            brightBlack = 90,
            brightRed = 91,
            brightGreen = 92,
            brightYellow = 93,
            brightMagenta = 95,
            brightCyan = 96,
            brightWhite = 97,
        };

        enum class background: unsigned char {
            black = 40,
            red = 41,
            green = 42,
            yellow = 43,
            blue = 44,
            magenta = 45,
            cyan = 46,
            white = 47,
            brightBlack = 100,
            brightRed = 101,
            brightGreen = 102,
            brightYellow = 103,
            brightMagenta = 105,
            brightCyan = 106,
            brightWhite = 107,
        };

        const ColorEndType colorEnd;
    }

    namespace extended {
        struct foreground {
            unsigned char value;
            foreground(unsigned char val): value(val) {}
            operator int() {
                return value;
            }
        };

        struct background {
            unsigned char value;
            background(unsigned char val): value(val) {}
            operator int() {
                return value;
            }
        };

        const ColorEndType colorEnd;
    };
}

std::ostream& operator<<(std::ostream& os, colorize::standart::foreground color) {
    return os << "\033[" << (int) color << 'm';
}
std::ostream& operator<<(std::ostream& os, colorize::standart::background color) {
    return os << "\033[" << (int) color << 'm';
}

std::ostream& operator<<(std::ostream& os, colorize::extended::foreground color) {
    return os << "\033[38;5;" << (int) color << 'm';
}
std::ostream& operator<<(std::ostream& os, colorize::extended::background color) {
    return os << "\033[48;5;" << (int) color << 'm';
}

std::ostream& operator<<(std::ostream& os, colorize::ColorEndType color) {
    return os << "\033[0m";
}
