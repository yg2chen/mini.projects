#pragma once

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/Mangling.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/Shared/ExecutorSymbolDef.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/Error.h"
#include <memory>

namespace llvm {
namespace orc {
class KaleidoscopeJIT {
  private:
    // provides context for jit running code
    // string pool, global mutex, error reporting facilities etc.
    std::unique_ptr<ExecutionSession> ES;

    // can be used to add object file to jit
    RTDyldObjectLinkingLayer ObjectLayer;

    // can be used to add LLVM Modules
    IRCompileLayer CompileLayer;

    DataLayout DL;
    MangleAndInterner Mangle;

    // LLVMContext that clients will use when building IR files for jit
    // it's actually LLVMContext itself
    // ThreadSafeContext ensures that this container can be safely accessed and
    // modified by multi-threads
    /*ThreadSafeContext Ctx;*/

    // JITDylib used to organized the generated code, and also allowing for
    // dynamic linking
    // JITDyLib acts as a dynamic library within JIT,  also handles symbol
    // resolution
    JITDylib &MainJD;

  public:
    // JITTargetMachineBuilder
    // handles tasks like detecting host target triple, setting cpu features,
    // etc.
    KaleidoscopeJIT(std::unique_ptr<ExecutionSession> ES,
                    JITTargetMachineBuilder JTMB, DataLayout DL)
        : ES(std::move(ES)), DL(std::move(DL)), Mangle(*this->ES, this->DL),
          ObjectLayer(
              *this->ES,
              []() { return std::make_unique<SectionMemoryManager>(); }),
          CompileLayer(*this->ES, ObjectLayer,
                       std::make_unique<ConcurrentIRCompiler>(std::move(JTMB))),
          MainJD(this->ES->createBareJITDylib("<main>")) {
        MainJD.addGenerator(
            cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
                DL.getGlobalPrefix())));
    };

    ~KaleidoscopeJIT() {
        if (auto Err = ES->endSession()) {
            ES->reportError(std::move(Err));
        }
    }

    static Expected<std::unique_ptr<KaleidoscopeJIT>> Create();

    const DataLayout &getDataLayout();

    JITDylib &getMainJITDylib();

    Error addModule(ThreadSafeModule TSM, ResourceTrackerSP RT = nullptr);

    Expected<ExecutorSymbolDef> lookup(StringRef Name);
};

} // end namespace orc
} // end namespace llvm
