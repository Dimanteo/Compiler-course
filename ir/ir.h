#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "mdbset_demo.h"

llvm::Function *buildIR(llvm::Module *module, llvm::IRBuilder<> &builder);