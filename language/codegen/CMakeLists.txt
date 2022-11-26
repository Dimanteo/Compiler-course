
find_package(BISON REQUIRED)
find_package(FLEX)

set(FLEX_INPUT tokenizer.lex)
set(FLEX_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/tokenizer.cpp")
set(BISON_INPUT parser.y)
set(BISON_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/parser.cpp")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

FLEX_TARGET(Tokenizer ${FLEX_INPUT} ${FLEX_OUTPUT})
BISON_TARGET(Parser ${BISON_INPUT} ${BISON_OUTPUT})
ADD_FLEX_BISON_DEPENDENCY(Tokenizer Parser)

add_library(codegen STATIC ${BISON_Parser_OUTPUTS} ${FLEX_Tokenizer_OUTPUTS})
target_link_libraries(codegen ${FLEX_LIBRARIES})