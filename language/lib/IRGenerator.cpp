#include "IRGenerator.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

namespace kolang {

// Syntax sugar for lowering std::variant
#define LLV(v) std::get<llvm::Value *>(v)

IRGenerator::IRGenerator() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>(genModuleID(), *context);
    builder = std::make_unique<IRBuilder<>>(*context);
    FunctionType *voidFuncType = FunctionType::get(builder->getVoidTy(), false);
    Function *mainFunc =
        Function::Create(voidFuncType, Function::ExternalLinkage,
                         getEntryFuncName(), module.get());
    BasicBlock *bb = llvm::BasicBlock::Create(*context, "BB0", mainFunc);
    builder->SetInsertPoint(bb);
}

void IRGenerator::executeAndFreeModule() {
    Function *mainFunc = module->getFunction(getEntryFuncName());
    Module *tmp_ptr = module.release();
    module = std::make_unique<Module>(genModuleID(), *context);
    ExecutionEngine *ee =
        EngineBuilder(std::unique_ptr<Module>(tmp_ptr)).create();
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

ASTNode IRGenerator::genAdd(ASTNode lhs, ASTNode rhs) {
    return builder->CreateAdd(LLV(lhs), LLV(rhs));
}

ASTNode IRGenerator::genSub(ASTNode lhs, ASTNode rhs) {
    return builder->CreateSub(LLV(lhs), LLV(rhs));
}

ASTNode IRGenerator::genMul(ASTNode lhs, ASTNode rhs) {
    return builder->CreateMul(LLV(lhs), LLV(rhs));
}

ASTNode IRGenerator::genDiv(ASTNode lhs, ASTNode rhs) {
    return builder->CreateUDiv(LLV(lhs), LLV(rhs));
}

ASTNode IRGenerator::genNumber(int val) { return builder->getInt64(val); }

ASTNode IRGenerator::genRet(ASTNode val) { return builder->CreateRet(LLV(val)); }

ASTNode IRGenerator::genRet() { return builder->CreateRetVoid(); }

ASTNode IRGenerator::genLocalAlloc() {
    return builder->CreateAlloca(builder->getInt64Ty());
}

ASTNode IRGenerator::genGlobalAlloc(std::string &name) {
    return module->getOrInsertGlobal(name, builder->getInt64Ty());
}

ASTNode IRGenerator::genLoad(ASTNode ptr) {
    return builder->CreateLoad(builder->getInt64Ty(), LLV(ptr));
}

void IRGenerator::genStore(ASTNode ptr, ASTNode value) {
    builder->CreateStore(LLV(value), LLV(ptr));
}

}; // namespace kolang