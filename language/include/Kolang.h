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

}; // namespace kolang

#endif