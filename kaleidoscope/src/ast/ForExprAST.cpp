
#include "ForExprAST.h"
#include "kaleidoscope.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

// Output for-loop as:
//   var = alloca double
//   ...
//   start = startexpr
//   store start -> var
//   goto loop
// loop:
//   ...
//   bodyexpr
//   ...
// loopend:
//   step = stepexpr
//   endcond = endexpr
//
//   curvar = load var
//   nextvar = curvar + step
//   store nextvar -> var
//   br endcond, loop, endloop
// outloop:
llvm::Value *ForExprAST::codegen() {
    llvm::Function *TheFunction = Builder->GetInsertBlock()->getParent();

    // create an alloca
    llvm::AllocaInst *Alloca = CreateEntryBlockAlloc(TheFunction, VarName);

    // Emit the start code first, without 'variable' in scope.
    llvm::Value *StartVal = Start->codegen();
    if (!StartVal)
        return nullptr;

    // store the value into the alloca
    Builder->CreateStore(StartVal, Alloca);

    // Make the new basic block for the loop header, inserting after current
    // block.
    llvm::BasicBlock *LoopBB =
        llvm::BasicBlock::Create(*TheContext, "loop", TheFunction);

    // Insert an explicit fall through from the current block to the LoopBB.
    Builder->CreateBr(LoopBB);

    // Start insertion in LoopBB.
    Builder->SetInsertPoint(LoopBB);

    // Within the loop, the variable is defined equal to the PHI node.  If it
    // shadows an existing variable, we have to restore it, so save it now.
    llvm::AllocaInst *OldVal = NamedValues[VarName];
    NamedValues[VarName] = Alloca;

    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but
    // don't allow an error.
    if (!Body->codegen())
        return nullptr;

    // Emit the step value.
    llvm::Value *StepVal = nullptr;
    if (Step) {
        StepVal = Step->codegen();
        if (!StepVal)
            return nullptr;
    } else {
        // If not specified, use 1.0.
        StepVal = llvm::ConstantFP::get(*TheContext, llvm::APFloat(1.0));
    }

    // Compute the end condition.
    llvm::Value *EndCond = End->codegen();
    if (!EndCond)
        return nullptr;

    // reload, inc., and restore the alloc.
    llvm::Value *CurVal = Builder->CreateLoad(
        llvm::Type::getDoubleTy(*TheContext), Alloca, VarName.c_str());
    llvm::Value *NextVal = Builder->CreateFAdd(CurVal, StepVal, "nextvar");
    Builder->CreateStore(NextVal, Alloca);

    // Convert condition to a bool by comparing non-equal to 0.0.
    EndCond = Builder->CreateFCmpONE(
        EndCond, llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0)),
        "loopcond");

    // Create the "after loop" block and insert it.
    llvm::BasicBlock *AfterBB =
        llvm::BasicBlock::Create(*TheContext, "afterloop", TheFunction);

    // Insert the conditional branch into the end of LoopEndBB.
    Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

    // Any new code will be inserted in AfterBB.
    Builder->SetInsertPoint(AfterBB);

    // Restore the unshadowed variable.
    if (OldVal)
        NamedValues[VarName] = OldVal;
    else
        NamedValues.erase(VarName);

    // for expr always returns 0.0.
    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*TheContext));
}
