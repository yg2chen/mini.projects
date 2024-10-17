#pragma once

#include "ExprAST.h"
#include "kaleidoscope.h"

class NumberExprAST : public ExprAST {
    double Val;

  public:
    NumberExprAST(double Val) : Val(Val) {}
    llvm::Value *codegen() override;
};
