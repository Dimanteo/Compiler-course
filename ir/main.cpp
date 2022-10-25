#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

#include "ir.h"

void dumpModuleIR(const llvm::Module &module, std::ostream &outs) {
    outs << "## [LLVM IR] ##\n";
    std::string out_str;
    llvm::raw_string_ostream os(out_str);
    module.print(os, nullptr);
    os.flush();
    outs << out_str;
}

int main() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    llvm::LLVMContext context;
    llvm::Module *module = new llvm::Module("Mandelbrot", context);
    llvm::IRBuilder<> builder(context);

    llvm::Function *mainFunc = buildIR(module, builder);

    dumpModuleIR(*module, std::cout);

    std::cout << "\nExecuting IR\n";

    llvm::ExecutionEngine *ee = 
        llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module)).create();

    std::unordered_map<std::string, void*> func_map;
    #define MAPF(fname) \
        func_map[#fname] = reinterpret_cast<void*>(fname)
    MAPF(initPixelArray);
    MAPF(initWindow);
    MAPF(isOpen);
    MAPF(handleWindowEvents);
    MAPF(zoom);
    MAPF(drawMandelbrotSet);
    MAPF(draw);
    MAPF(display);
    MAPF(destrPixelArray);
    MAPF(destrWindow);
    #undef MAPF
    ee->InstallLazyFunctionCreator(
        [&func_map](const std::string &fnName) -> void* {
            return func_map[fnName];
        }
    );

    ee->finalizeObject();

    std::vector<llvm::GenericValue> noargs;
    llvm::GenericValue v = ee->runFunction(mainFunc, noargs);

    std::cout << "\nExecution done\n";

    return 0;
}
