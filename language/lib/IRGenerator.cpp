#include "IRGenerator.h"
#include "AST.h"
#include "CompilerCore.h"
#include "libko.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
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
    std::unordered_map<std::string, void *> func_map;
    func_map["print"] = reinterpret_cast<void *>(__kolang_print);
    func_map["scan"] = reinterpret_cast<void *>(__kolang_scan);
    ee->InstallLazyFunctionCreator(
        [&func_map](const std::string fn_name) -> void * {
            return func_map[fn_name];
        });
    ee->finalizeObject();
    std::vector<llvm::GenericValue> noargs;
    GenericValue v = ee->runFunction(mainFunc, noargs);
    std::cout << "Result : " << v.IntVal.getZExtValue() << "\n";
}

void IRGenerator::dump(std::ostream &outs) {
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
    FunctionType *start_ty = FunctionType::get(builder->getInt64Ty(), false);
    Function *start_f = Function::Create(start_ty, Function::ExternalLinkage,
                                         Start_func_name, *module);
    BasicBlock *bb = BasicBlock::Create(*context, getBBName(), start_f);
    builder->SetInsertPoint(bb);
}

void IRGenerator::genStartEpilogue() {
    CompilerCore &cc = CompilerCore::getCCore();
    Function *start_f = module->getFunction(Start_func_name);
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
    IRValue mul_val = builder->CreateMul(lhs, rhs);
    return builder->CreateSDiv(mul_val, builder->getInt64(kolang::PRECISION));
}

IRValue IRGenerator::genDiv(IRValue lhs, IRValue rhs) {
    IRValue div_val = builder->CreateSDiv(lhs, rhs);
    return builder->CreateMul(div_val, builder->getInt64(kolang::PRECISION));
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
                               const std::vector<TypeIDNode *> &params) {
    std::vector<Type *> par_types;
    std::vector<strid_t> par_ids;
    for (TypeIDNode *node : params) {
        par_ids.push_back(node->getID());
        TYPE_ID type = node->getType();
        switch (type) {
        case TYPE_ID::INT64:
            par_types.push_back(builder->getInt64Ty());
            break;
        case TYPE_ID::ARR_INT64:
            par_types.push_back(
                ArrayType::get(builder->getInt64Ty(), node->getSize())
                    ->getPointerTo());
            break;
        default:
            llvm_unreachable("Unknown argument type");
            break;
        }
    }
    FunctionType *f_type =
        FunctionType::get(builder->getInt64Ty(), par_types, false);
    Function *func =
        Function::Create(f_type, Function::ExternalLinkage, name, *module);
    BasicBlock *bb = BasicBlock::Create(*context, getBBName(), func);
    builder->SetInsertPoint(bb);
    CompilerCore &cc = CompilerCore::getCCore();
    cc.populateAvailableVariables(par_ids);
    for (size_t i = 0; i < params.size(); ++i) {
        TYPE_ID arg_ty = params[i]->getType();
        strid_t arg_id = par_ids[i];
        if (isVariableType(arg_ty)) {
            IRValue argmem = allocateLocal();
            genStore(argmem, func->getArg(i));
            cc.addIDValueMapping(arg_id, argmem);
        } else if (isArrayType(arg_ty)) {
            // Arrays are passed as pointer
            cc.addIDValueMapping(arg_id, func->getArg(i));
        }
    }
}

IRValue IRGenerator::genCall(const std::string &name,
                             const std::vector<IRValue> &args) {
    Function *callee = module->getFunction(name);
    if (!callee) {
        // Form function prototype and hope that it will be linked later
        std::vector<llvm::Type *> args_types;
        for (IRValue arg : args) {
            args_types.push_back(arg->getType());
        }
        FunctionType *calee_ty =
            FunctionType::get(builder->getInt64Ty(), args_types, false);
        callee = Function::Create(calee_ty, Function::ExternalLinkage, name,
                                  *module);
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

void IRGenerator::genWhile(ASTNode *condition, ASTNode *body) {
    CompilerCore &cc = CompilerCore::getCCore();
    BasicBlock *intro = builder->GetInsertBlock();
    Function *curr_func = intro->getParent();
    BasicBlock *loop_bb = BasicBlock::Create(*context, getBBName(), curr_func);
    BasicBlock *outro = BasicBlock::Create(*context, getBBName(), curr_func);
    IRValue cond_val = condition->emit();
    builder->CreateCondBr(cond_val, loop_bb, outro);
    cc.enterScope();
    builder->SetInsertPoint(loop_bb);
    body->emit();
    cond_val = condition->emit();
    builder->CreateCondBr(cond_val, loop_bb, outro);
    cc.leaveScope();
    builder->SetInsertPoint(outro);
}

IRValue IRGenerator::genArrayDef(numb_t size) {
    ArrayType *arr_ty = ArrayType::get(builder->getInt64Ty(), size);
    IRValue arr = builder->CreateAlloca(arr_ty);
    return arr;
}

IRValue IRGenerator::genArrayAccess(strid_t id, IRValue index) {
    CompilerCore &cc = CompilerCore::getCCore();
    IRValue arr = cc.lookup(id);
    assert(arr->getType()->getContainedType(0)->isArrayTy() &&
           "Incorrect value type");
    return builder->CreateGEP(arr->getType()->getContainedType(0), arr,
                              {builder->getInt64(0), index});
}

IRValue IRGenerator::normalize(IRValue val) {
    return builder->CreateSDiv(val, builder->getInt64(kolang::PRECISION));
}

}; // namespace kolang