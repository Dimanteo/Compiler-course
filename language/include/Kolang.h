#ifndef KOLANG_H
#define KOLANG_H

#include <cstdint>

namespace kolang {

// User program entry point
constexpr const char *Entry_func_name = "main";
// STD lib entry point
constexpr const char *Start_func_name = "__kolang_start";
using strid_t = uint64_t;
using numb_t = int64_t;

constexpr numb_t PRECISION = 1000000;

constexpr double normalize(numb_t val) {
    return static_cast<double>(val) / static_cast<double>(kolang::PRECISION);
}

constexpr numb_t encode(double value) {
     return static_cast<numb_t>(value * kolang::PRECISION);
}

}; // namespace kolang

#endif