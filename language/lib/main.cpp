#include "parser.hpp"
#include "CompilerCore.h"

#include <iostream>

void parser_main(int argc, char **argv);

int main(int argc, char **argv) {
    parser_main(argc, argv);
    codak::CompilerCore &cc = codak::CompilerCore::getCCore();
    cc.getIRG().dump(std::cout);
    cc.getIRG().executeAndFreeModule();
    return 0;
}