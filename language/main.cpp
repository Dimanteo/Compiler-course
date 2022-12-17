#include "CompilerCore.h"
#include "parser.hpp"

#include <iostream>
#include <fstream>

bool parser_main(int argc, char **argv);

int main(int argc, char **argv) {
    if (parser_main(argc, argv)) {
        std::cerr << "Compilation failed. Exiting.\n";
        return EXIT_FAILURE;
    }
    kolang::CompilerCore &cc = kolang::CompilerCore::getCCore();
    cc.generateIR();
    if (argc > 2) {
        std::ofstream outs(argv[2]);
        cc.getIRG().dump(outs);
    } else {
        cc.getIRG().dump(std::cout);
        cc.getIRG().executeAndFreeModule();
    }
    return 0;
}