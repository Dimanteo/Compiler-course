add_library(demo_graphics mdbset_demo.cpp graphics.cpp)

target_link_libraries(demo_graphics sfml-graphics sfml-window sfml-system)