
#include "BinaryExprAST.h"

llvm::Value *BinaryExprAST::codegen() {
    llvm::Value *L = LHS->codegen();
    llvm::Value *R = RHS->codegen();
    if (!L || !R)
        return nullptr;

    switch (Op) {
    case '+':
        return Builder->CreateFAdd(L, R, "addtmp");
    case '-':
        return Builder->CreateFSub(L, R, "subtmp");
    case '*':
        return Builder->CreateFMul(L, R, "multmp");
    case '<':
        L = Builder->CreateFCmpULT(L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
        return Builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*TheContext),
                                     "booltmp");
    default:
        break;
    }

    llvm::Function *F = getFunction(std::string("binary") + Op);
    assert(F && "binary operation not found");

    llvm::Value *Ops[2] = {L, R};
    return Builder->CreateCall(F, Ops, "binop");
}
