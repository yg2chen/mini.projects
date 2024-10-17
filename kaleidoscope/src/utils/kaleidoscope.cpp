#include "kaleidoscope.h"

// This is an object that owns LLVM core data structures
std::unique_ptr<llvm::LLVMContext> TheContext;

// This is a helper object that makes easy to generate LLVM instructions
std::unique_ptr<llvm::IRBuilder<>> Builder;

// This is an LLVM construct that contains functions and global variables
std::unique_ptr<llvm::Module> TheModule;

// This map keeps track of which values are defined in the current scope
std::map<std::string, llvm::Value *> NamedValues;
