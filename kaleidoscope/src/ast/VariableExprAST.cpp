#include "VariableExprAST.h"

llvm::Value *VariableExprAST::codegen() {
    llvm::Value *V = NamedValues[Name];
    if (!V)
        LogErrorV("Unknown variable name");
    return V;
}
