#include "IRGenerator.h"
#include "AST.h"
#include "CompilerCore.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

namespace kolang {

IRGenerator::IRGenerator() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>(genModuleID(), *context);
    builder = std::make_unique<IRBuilder<>>(*context);
}

void IRGenerator::executeAndFreeModule() {
    Function *mainFunc = module->getFunction(Start_func_name);
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

void IRGenerator::genStartPrologue() {
    CompilerCore &cc = CompilerCore::getCCore();
    cc.enterScope();
    assert(cc.inGlobalScope() && "Unexpected scope depth");
    llvm::FunctionType *start_ty =
        FunctionType::get(builder->getInt64Ty(), false);
    llvm::Function *start_f = Function::Create(
        start_ty, Function::ExternalLinkage, Start_func_name, *module);
    BasicBlock *bb = BasicBlock::Create(*context, getBBName(), start_f);
    builder->SetInsertPoint(bb);
}

void IRGenerator::genStartEpilogue() {
    CompilerCore &cc = CompilerCore::getCCore();
    llvm::Function *start_f = module->getFunction(Start_func_name);
    assert(start_f && "Start function not found");
    builder->SetInsertPoint(&start_f->getEntryBlock());
    IRValue retval = genCall(Entry_func_name);
    genReturn(retval);
    assert(cc.inGlobalScope() && "Unexpected scope depth");
    cc.leaveScope();
}

void IRGenerator::emitAST(ASTNode *entry) { entry->emit(); }

IRValue IRGenerator::genAdd(IRValue lhs, IRValue rhs) {
    return builder->CreateAdd(lhs, rhs);
}

IRValue IRGenerator::genSub(IRValue lhs, IRValue rhs) {
    return builder->CreateSub((lhs), (rhs));
}

IRValue IRGenerator::genMul(IRValue lhs, IRValue rhs) {
    return builder->CreateMul(lhs, rhs);
}

IRValue IRGenerator::genDiv(IRValue lhs, IRValue rhs) {
    return builder->CreateUDiv(lhs, rhs);
}

IRValue IRGenerator::genNumber(numb_t val) { return builder->getInt64(val); }

void IRGenerator::genReturn(IRValue val) { builder->CreateRet(val); }

void IRGenerator::genReturn() { genReturn(genNumber(0)); }

IRValue IRGenerator::allocateLocal() {
    return builder->CreateAlloca(builder->getInt64Ty());
}

IRValue IRGenerator::allocateGlobal(const std::string &name) {
    module->getOrInsertGlobal(name, builder->getInt64Ty());
    return module->getNamedGlobal(name);
}

IRValue IRGenerator::genLoad(IRValue ptr) {
    return builder->CreateLoad(builder->getInt64Ty(), ptr);
}

void IRGenerator::genStore(IRValue ptr, IRValue value) {
    builder->CreateStore(value, ptr);
}

void IRGenerator::declFunction(const std::string &name) {
    FunctionType *f_type = FunctionType::get(builder->getInt64Ty(), false);
    Function *func =
        Function::Create(f_type, Function::ExternalLinkage, name, *module);
    BasicBlock *bb = BasicBlock::Create(*context, getBBName(), func);
    builder->SetInsertPoint(bb);
}

void IRGenerator::declFunction(const std::string &name,
                               const std::vector<strid_t> &params) {
    std::vector<Type *> par_types(params.size(), builder->getInt64Ty());
    FunctionType *f_type =
        FunctionType::get(builder->getInt64Ty(), par_types, false);
    Function *func =
        Function::Create(f_type, Function::ExternalLinkage, name, *module);
    BasicBlock *bb = BasicBlock::Create(*context, getBBName(), func);
    builder->SetInsertPoint(bb);
    CompilerCore &cc = CompilerCore::getCCore();
    cc.populateAvailableVariables(params);
    for (size_t i = 0; i < params.size(); ++i)  {
        strid_t arg_id = params[i];
        IRValue argmem = allocateLocal();
        genStore(argmem, func->getArg(i));
        cc.addIDValueMapping(arg_id, argmem);
    }
}

IRValue IRGenerator::genCall(const std::string &name,
                             const std::vector<IRValue> &args) {
    Function *callee = module->getFunction(name);
    if (!callee) {
        std::cerr << "Undefined function referenced\n";
        return IRGenerator::NIL();
    }
    IRValue val = IRGenerator::NIL();
    if (args.size()) {
        val = builder->CreateCall(callee, args);
    } else {
        val = builder->CreateCall(callee);
    }
    return val;
}

IRValue IRGenerator::genCall(const std::string &name) {
    return genCall(name, std::vector<IRValue>());
}

IRValue IRGenerator::genLT(IRValue lhs, IRValue rhs) {
    return builder->CreateICmpSLT(lhs, rhs);
}

IRValue IRGenerator::genGT(IRValue lhs, IRValue rhs) {
    return builder->CreateICmpSGT(lhs, rhs);
}

IRValue IRGenerator::genEQ(IRValue lhs, IRValue rhs) {
    return builder->CreateICmpEQ(lhs, rhs);
}

IRValue IRGenerator::genNEQ(IRValue lhs, IRValue rhs) {
    return builder->CreateICmpNE(lhs, rhs);
}

IRValue IRGenerator::genLEQ(IRValue lhs, IRValue rhs) {
    return builder->CreateICmpSLE(lhs, rhs);
}

IRValue IRGenerator::genGEQ(IRValue lhs, IRValue rhs) {
    return builder->CreateICmpSGE(lhs, rhs);
}

void IRGenerator::genIf(IRValue condition, ASTNode *true_path) {
    CompilerCore &cc = CompilerCore::getCCore();
    BasicBlock *intro = builder->GetInsertBlock();
    Function *curr_func = intro->getParent();
    BasicBlock *true_bb = BasicBlock::Create(*context, getBBName(), curr_func);
    BasicBlock *outro = BasicBlock::Create(*context, getBBName(), curr_func);
    builder->CreateCondBr(condition, true_bb, outro);
    cc.enterScope();
    builder->SetInsertPoint(true_bb);
    true_path->emit();
    builder->CreateBr(outro);
    cc.leaveScope();
    builder->SetInsertPoint(outro);
}

}; // namespace kolang