#pragma once

// function definition
#include "ExprAST.h"
#include "PrototypeAST.h"
#include "kaleidoscope.h"
#include "logger.h"
#include <llvm/IR/Function.h>
#include <memory>
class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
    llvm::Function *codegen();
};
