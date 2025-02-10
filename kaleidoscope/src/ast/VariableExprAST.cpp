#include "VariableExprAST.h"

llvm::Value *VariableExprAST::codegen() {
    llvm::AllocaInst *V = NamedValues[Name];
    if (!V)
        LogErrorV("Unknown variable name");
    // load the value
    return Builder->CreateLoad(V->getAllocatedType(), V, Name.c_str());
}
