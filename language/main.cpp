#include "CompilerCore.h"
#include "codegen/parser.hpp"

#include <iostream>

void parser_main(int argc, char **argv);

int main(int argc, char **argv) {
    parser_main(argc, argv);
    kolang::IRGenerator &irg = kolang::CompilerCore::getCCore().getIRG();
    irg.dump(std::cout);
    irg.executeAndFreeModule();
    return 0;
}