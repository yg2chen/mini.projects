#include "kaleidoscopejit.h"
namespace llvm {
namespace orc {

Expected<std::unique_ptr<KaleidoscopeJIT>> KaleidoscopeJIT::Create() {
    auto EPC = SelfExecutorProcessControl::Create();
    if (!EPC) {
        return EPC.takeError();
    }

    auto ES = std::make_unique<ExecutionSession>(std::move(*EPC));

    JITTargetMachineBuilder JTMB(
        ES->getExecutorProcessControl().getTargetTriple());

    auto DL = JTMB.getDefaultDataLayoutForTarget();
    if (!DL) {
        return DL.takeError();
    }

    return std::make_unique<KaleidoscopeJIT>(std::move(ES), std::move(JTMB),
                                             std::move(*DL));
}

const DataLayout &KaleidoscopeJIT::getDataLayout() { return DL; }

JITDylib &KaleidoscopeJIT::getMainJITDylib() { return MainJD; }

Error KaleidoscopeJIT::addModule(ThreadSafeModule TSM, ResourceTrackerSP RT) {
    if (!RT) {
        RT = MainJD.getDefaultResourceTracker();
    }
    return CompileLayer.add(RT, std::move(TSM));
}

Expected<ExecutorSymbolDef> KaleidoscopeJIT::lookup(StringRef Name) {
    return ES->lookup({&MainJD}, Mangle(Name.str()));
}

} // end namespace orc
} // end namespace llvm
