cmake_minimum_required(VERSION 3.0.0)
project(language VERSION 0.1.0)

find_package(BISON REQUIRED)
find_package(FLEX)

set(FLEX_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/tokenizer.lex")
set(FLEX_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/tokenizer.cpp")
set(BISON_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/parser.y")
set(BISON_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/parser.cpp")

FLEX_TARGET(Tokenizer ${FLEX_INPUT} ${FLEX_OUTPUT})
BISON_TARGET(Parser ${BISON_INPUT} ${BISON_OUTPUT})
ADD_FLEX_BISON_DEPENDENCY(Tokenizer Parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(language main.cpp ${BISON_Parser_OUTPUTS} ${FLEX_Tokenizer_OUTPUTS})
target_link_libraries(language ${FLEX_LIBRARIES})