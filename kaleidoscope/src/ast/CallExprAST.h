#pragma once

#include "ExprAST.h"
#include "kaleidoscope.h"
#include "logger.h"

// function calls
class CallExprAST : public ExprAST {
    std::vector<std::unique_ptr<ExprAST>> Args;
    std::string Callee;

  public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}
    llvm::Value *codegen() override;
};
