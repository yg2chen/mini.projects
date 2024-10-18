#include "FunctionAST.h"
#include "kaleidoscope.h"
#include <llvm/IR/BasicBlock.h>

llvm::Function *FunctionAST::codegen() {
    auto &P = *Proto;
    FunctionProtos[Proto->getName()] = std::move(Proto);
    llvm::Function *TheFunction = getFunction(P.getName());

    if (!TheFunction)
        return nullptr;

    // Create a new basic block to start insertion into.
    llvm::BasicBlock *BB =
        llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
    NamedValues.clear();
    for (auto &Arg : TheFunction->args())
        NamedValues[std::string(Arg.getName())] = &Arg;

    if (llvm::Value *RetVal = Body->codegen()) {
        // Finish off the function.
        Builder->CreateRet(RetVal);

        // Validate the generated code, checking for consistency.
        verifyFunction(*TheFunction);
        // optimizations
        TheFPM->run(*TheFunction, *TheFAM);

        return TheFunction;
    }

    // Error reading body, remove function.
    TheFunction->eraseFromParent();
    return nullptr;
}
