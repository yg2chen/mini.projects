#pragma once

#include "ExprAST.h"
#include "kaleidoscope.h"
#include "logger.h"

class VariableExprAST : public ExprAST {
    std::string Name;

  public:
    VariableExprAST(const std::string Name) : Name(Name) {}
    llvm::Value *codegen() override;
};
