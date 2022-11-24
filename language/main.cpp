#include <parser.hpp>
#include "IRGenerator.h"

#include <iostream>

void parser_main(int argc, char **argv);

int main(int argc, char **argv) {
    parser_main(argc, argv);
    codak::IRGenerator &irg = codak::IRGenerator::getGenerator();
    irg.dump(std::cout);
    irg.executeAndFreeModule();
    return 0;
}