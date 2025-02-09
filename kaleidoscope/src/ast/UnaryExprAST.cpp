#include "UnaryExprAST.h"
#include "kaleidoscope.h"
#include "llvm/IR/Function.h"

llvm::Value *UnaryExprAST::codegen() {
    llvm::Value *OperandV = Operand->codegen();
    if (!OperandV) {
        return nullptr;
    }

    llvm::Function *F = getFunction(std::string("unary") + Opcode);
    return Builder->CreateCall(F, OperandV, "unop");
}
