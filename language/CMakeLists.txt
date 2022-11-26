cmake_minimum_required(VERSION 3.0.0)
project(kolang VERSION 0.1.0)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/codegen)

add_executable(kolang main.cpp)
add_subdirectory(lib)
add_subdirectory(codegen)
target_link_libraries(kolang codegen lib)