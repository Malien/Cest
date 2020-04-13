#pragma once
#include <string>
#include <chrono>
#include <iomanip>
#include "colorize.hpp"
#include "time_util.hpp"

namespace cest {
    namespace message {
        struct _msg {
            const std::string_view& name;
            const char* filename;
            int line;
            std::chrono::time_point<std::chrono::steady_clock> start;
        };

        struct pass: public _msg {};
        struct fail: public _msg {};
    }
}

inline std::ostream& operator<<(std::ostream& os, const cest::message::pass& msg) {
    using namespace colorize::standart;
    return os << foreground::brightGreen << "✓" << colorize::end << ' ' << msg.name << ' ' 
              << foreground::brightBlack << msg.filename << ':' << msg.line << colorize::end 
              << foreground::yellow << std::setprecision(2) << " (" << time_util::timeDiff(msg.start) << ')' << colorize::end;
}

inline std::ostream& operator<<(std::ostream& os, const cest::message::fail& msg) {
    using namespace colorize::standart;
    return os << foreground::brightRed << 'x' << colorize::end << ' ' << msg.name << ' '
              << foreground::brightBlack << msg.filename << ':' << msg.line << colorize::end
              << foreground::yellow << " (" << time_util::timeDiff(msg.start) << ')' << colorize::end; 
}