# LLVM practice

This is an educational project. The goal was to implement programming language and custom architecture backend using LLVM framework.

## Kolang language
Uses Flex+Bison for frontend and LLVM IR. See implenentation in folder `language`.

## Koda backend
In llvm-project submodule implementation of custom (RISC-V in fact) architecture can be found. `sim2022` is a RISC-V simulator that can execute produced binaries.
