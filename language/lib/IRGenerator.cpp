#include "IRGenerator.h"
#include "Frame.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

namespace kolang {

// Syntax sugar for extracting llvm::Value
#define LLV(v) ASTNode::asPtr(v.get())

IRGenerator::IRGenerator() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>(genModuleID(), *context);
    builder = std::make_unique<IRBuilder<>>(*context);
    genFunction(getStartFuncName(), ASTNode::getNIL());
}

void IRGenerator::executeAndFreeModule() {
    Function *mainFunc = module->getFunction(getStartFuncName());
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

ASTNode IRGenerator::genRet(ASTNode val) {
    return builder->CreateRet(LLV(val));
}

ASTNode IRGenerator::genRet() { return builder->CreateRetVoid(); }

ASTNode IRGenerator::genLocalAlloc() {
    return builder->CreateAlloca(builder->getInt64Ty());
}

ASTNode IRGenerator::genGlobalAlloc(std::string &name) {
    module->getOrInsertGlobal(name, builder->getInt64Ty());
    return module->getNamedGlobal(name);
}

ASTNode IRGenerator::genLoad(ASTNode ptr) {
    return builder->CreateLoad(builder->getInt64Ty(), LLV(ptr));
}

void IRGenerator::genStore(ASTNode ptr, ASTNode value) {
    builder->CreateStore(LLV(value), LLV(ptr));
}

void IRGenerator::genFunction(const std::string &name, ASTNode args) {
    std::vector<llvm::Type *> params;
    FunctionType *type = nullptr;
    if (!args.isNIL()) {
        for (size_t i = 0; i < args.getNumValues(); ++i) {
            params.push_back(builder->getInt64Ty());
        }
        type = FunctionType::get(builder->getInt64Ty(), params, false);
    } else {
        type = FunctionType::get(builder->getInt64Ty(), false);
    }
    Function *func =
        Function::Create(type, Function::ExternalLinkage, name, *module);
    BasicBlock *bb =
        BasicBlock::Create(*context, FunctionFrame::getBBname(0), func);
    assert(bb == &func->getEntryBlock());
    builder->SetInsertPoint(bb);
}

void IRGenerator::insertIn(Frame *frame) {
    builder->SetInsertPoint(frame->getCurrentBB());
}

bool IRGenerator::finalizeStartFunction() {
    llvm::Function *mainF =
        module->getFunction(IRGenerator::getEntryFuncName());
    if (!mainF) {
        std::cerr << "Compiler error. No \"main\" function.\n";
        return false;
    }
    llvm::Value *retval = builder->CreateCall(mainF);
    builder->CreateRet(retval);
    return true;
}

ASTNode IRGenerator::genCall(std::string &name, ASTNode args) {
    llvm::Function *callee = module->getFunction(name);
    llvm::Value *value = nullptr;
    if (args.isNIL()) {
        value = builder->CreateCall(callee);
    } else {
        std::vector<llvm::Value *> params;
        for (auto v : args.getValues()) {
            params.push_back(ASTNode::asPtr(v));
        }
        value = builder->CreateCall(callee, params);
    }
    return value;
}

#undef LLV

}; // namespace kolang