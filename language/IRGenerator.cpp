#include "IRGenerator.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

#include <memory>
#include <vector>
#include <string>
#include <iostream>

using namespace llvm;

namespace codak {

IRGenerator::IRGenerator() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>(genModuleID(), *context);
    builder = std::make_unique<IRBuilder<>>(*context);
    FunctionType *voidFuncType =
        FunctionType::get(builder->getVoidTy(), false);
    Function *mainFunc = Function::Create(
        voidFuncType, Function::ExternalLinkage, "main", module.get());
    BasicBlock *bb = llvm::BasicBlock::Create(*context, "BB0", mainFunc);
    builder->SetInsertPoint(bb);
}

void IRGenerator::executeAndFreeModule() {
    Function *mainFunc = module->getFunction("main");
    Module *tmp_ptr = module.release();
    module = std::make_unique<Module>(genModuleID(), *context);
    ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(tmp_ptr)).create();
    ee->finalizeObject();
    std::vector<llvm::GenericValue> noargs;
    GenericValue v = ee->runFunction(mainFunc, noargs);
    std::cout << "Result : " << v.IntVal.getZExtValue() << "\n";
}

void IRGenerator::dump(std::ostream &outs) {
    outs << "## [LLVM IR] ##\n";
    std::string out_str;
    llvm::raw_string_ostream os(out_str);
    module->print(os, nullptr);
    os.flush();
    outs << out_str;
}

Node IRGenerator::genAdd(Node lhs, Node rhs) {
    return builder->CreateAdd(lhs, rhs);
}

Node IRGenerator::genSub(Node lhs, Node rhs) {
    return builder->CreateSub(lhs, rhs);
}

Node IRGenerator::genMul(Node lhs, Node rhs) {
    return builder->CreateMul(lhs, rhs);
}

Node IRGenerator::genDiv(Node lhs, Node rhs) {
    return builder->CreateUDiv(lhs, rhs);
}

Node IRGenerator::genNumber(int val) {
    return builder->getInt64(val);
}

Node IRGenerator::genRet(Node val) {
    return builder->CreateRet(val);
}

Node IRGenerator::genRet() {
    return builder->CreateRetVoid();
}

}; // namespace codak