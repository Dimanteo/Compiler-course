#include "parser.hpp"
#include "CompilerCore.h"

#include <iostream>

bool parser_main(int argc, char **argv);

int main(int argc, char **argv) {
    if (parser_main(argc, argv)) {
        std::cerr << "Compilation failed. Exiting.\n";
        return EXIT_FAILURE;
    }
    kolang::CompilerCore &cc = kolang::CompilerCore::getCCore();
    cc.getIRG().dump(std::cout);
    cc.getIRG().executeAndFreeModule();
    return 0;
}