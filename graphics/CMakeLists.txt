cmake_minimum_required(VERSION 3.0.0)
project(Mandelbrot VERSION 0.1.0)


add_executable(fractal main.cpp)

# Add SFML
target_link_libraries(fractal sfml-graphics sfml-window sfml-system )