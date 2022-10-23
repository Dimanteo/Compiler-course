#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <string>
#include <memory>

#include "mdbset_demo.h"

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

    llvm::FunctionType *voidFuncType = 
        llvm::FunctionType::get(builder.getVoidTy(), false);
    llvm::Function *mainFunc = 
        llvm::Function::Create(voidFuncType, llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock *bb = llvm::BasicBlock::Create(context, "_start", mainFunc);

    builder.SetInsertPoint(bb);

    llvm::Function *demoStartFunc = 
        llvm::Function::Create(voidFuncType, llvm::Function::ExternalLinkage, "startMdbsetDemo", module);

    builder.CreateCall(demoStartFunc);
    builder.CreateRetVoid();

    dumpModuleIR(*module, std::cout);

    std::cout << "\nExecuting IR\n";

    llvm::ExecutionEngine *ee = 
        llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module)).create();

    ee->InstallLazyFunctionCreator(
        [](const std::string &fnName) -> void* {
            return reinterpret_cast<void*>(startMdbsetDemo);
        }
    );

    ee->finalizeObject();

    std::vector<llvm::GenericValue> noargs;
    llvm::GenericValue v = ee->runFunction(mainFunc, noargs);

    std::cout << "\nExecution done\n";

    return 0;
}
