#pragma once

#include "ExprAST.h"
#include "kaleidoscope.h"
#include "llvm/IR/IRBuilder.h"
#include <string>
#include <vector>

// prototype for function
// captures its name, its args
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

  public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args)) {}

    const std::string &getName() const { return Name; }
    llvm::Function *codegen();
};
