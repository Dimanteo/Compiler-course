add_library(demo_graphics graphics.cpp)
add_library(mdbset_demo mdbset_demo.cpp)

target_link_libraries(demo_graphics sfml-graphics sfml-window sfml-system)
target_link_libraries(mdbset_demo demo_graphics)