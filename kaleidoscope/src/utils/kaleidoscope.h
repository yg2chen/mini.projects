#pragma once

#include "PrototypeAST.h"
#include "kaleidoscope_jit.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include <map>
#include <memory>

class PrototypeAST; // forwarding, breaking circular dependency when compiling

// This is an object that owns LLVM core data structures
extern std::unique_ptr<llvm::LLVMContext> TheContext;

// This is a helper object that makes easy to generate LLVM instructions
extern std::unique_ptr<llvm::IRBuilder<>> Builder;

// This is an LLVM construct that contains functions and global variables
extern std::unique_ptr<llvm::Module> TheModule;

// This map keeps track of which values are defined in the current scope
extern std::map<std::string, llvm::Value *> NamedValues;

// LLVM Optimizations
extern std::unique_ptr<llvm::FunctionPassManager> TheFPM;
extern std::unique_ptr<llvm::LoopAnalysisManager> TheLAM;
extern std::unique_ptr<llvm::FunctionAnalysisManager> TheFAM;
extern std::unique_ptr<llvm::CGSCCAnalysisManager> TheCGAM;
extern std::unique_ptr<llvm::ModuleAnalysisManager> TheMAM;
extern std::unique_ptr<llvm::PassInstrumentationCallbacks> ThePIC;
extern std::unique_ptr<llvm::StandardInstrumentations> TheSI;
extern std::unique_ptr<llvm::orc::KaleidoscopeJIT> TheJIT;
extern std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
extern llvm::PassBuilder PB;
extern llvm::ExitOnError EOE;

llvm::Function *getFunction(std::string Name);
