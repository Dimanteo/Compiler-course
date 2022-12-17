# Kolang programming language

## Language description

Imperative programming language. Program consists of variables and functons. Entry point - function main.
Grammar:
* Function definition : 
```
name(arg1, arg2) 
{ body }
```
Body contains statements divided by ; (C-like)
* Comments separated by #
* Variable definitions `let name = 42`. All variables must be initialized. Variables inside function scope are local, outside all scopes - global.
* Arrays `let name[5]` - defines array of size 5.
* If statements `if (<condition>) { # executes if condition is true }`
* Only while loops for now `while (consition) { body }`
* Return statements `return val;`
* Function call `foo(arg1, arg2, arg3)`

For examples look in tests folder.

All variables are 64 bit integers. Arrays also contain 64 bit integers.

## How to use
```bash
mkdir build && cd build
cmake ..
make
./kolang <file>
```

Comand line args : 
* first arg - text file with kolang program
* second arg - output LLVM IR file
