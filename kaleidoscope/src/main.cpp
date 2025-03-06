#include "ast/FunctionAST.h"
#include "ast/PrototypeAST.h"
#include "kaleidoscopejit/kaleidoscopejit.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/parser.h"
#include "utils/kaleidoscope.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include <cstdio>
#include <memory>

using namespace llvm;

static void InitializeModuleAndPassManagers(void) {
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("KaleidoscopeJIT", *TheContext);
    TheModule->setDataLayout(TheJIT->getDataLayout());

    Builder = std::make_unique<IRBuilder<>>(*TheContext);
    TheFPM = std::make_unique<FunctionPassManager>();
    TheLAM = std::make_unique<LoopAnalysisManager>();
    TheFAM = std::make_unique<FunctionAnalysisManager>();
    TheCGAM = std::make_unique<CGSCCAnalysisManager>();
    TheMAM = std::make_unique<ModuleAnalysisManager>();
    ThePIC = std::make_unique<PassInstrumentationCallbacks>();
    TheSI = std::make_unique<StandardInstrumentations>(*TheContext,
                                                       /* DebugLogging */ true);
    TheSI->registerCallbacks(*ThePIC, TheMAM.get());

    /* promote allocas to registers, mem2reg*/
    TheFPM->addPass(PromotePass());
    /* "peephole" optimizations and bit-twiddling optimizations*/
    TheFPM->addPass(InstCombinePass());
    /* Reassociate expressions*/
    TheFPM->addPass(ReassociatePass());
    /* Eliminate Common SubExpressions(CSE)*/
    TheFPM->addPass(GVNPass());
    /* Simplify the Control Flow Graph*/
    TheFPM->addPass(SimplifyCFGPass());
    /* Register analysis passes*/
    PB.registerModuleAnalyses(*TheMAM);
    PB.registerFunctionAnalyses(*TheFAM);
    PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
}

static void HandleDefinition() {
    if (auto FnAST = ParseDefinition()) {
        if (auto *FnIR = FnAST->codegen()) {
            fprintf(stderr, "Read function definition:\n");
            FnIR->print(errs());
            fprintf(stderr, "\n");
            EOE(TheJIT->addModule(orc::ThreadSafeModule(
                std::move(TheModule), std::move(TheContext))));
            InitializeModuleAndPassManagers();
        }
    } else {
        getNextToken();
    }
}

static void HandleExtern() {
    if (auto ProtoAST = ParseExtern()) {
        if (auto *FnIR = ProtoAST->codegen()) {
            fprintf(stderr, "Read extern:\n");
            FnIR->print(errs());
            fprintf(stderr, "\n");
            FunctionProtos[ProtoAST->getName()] = std::move(ProtoAST);
        }
    } else {
        getNextToken();
    }
}

static void HandleTopLevelExpression() {
    if (auto FnAST = ParseTopLevelExpr()) {
        if (auto *FnIR = FnAST->codegen()) {
            fprintf(stderr, "Read top-level expression:\n");
            FnIR->print(errs());
            fprintf(stderr, "\n");
            // Create a ResourceTracker to track JIT'd memory allocated to our
            // anonymous expression -- that way we can free it after executing.
            auto RT = TheJIT->getMainJITDylib().createResourceTracker();

            auto TSM = llvm::orc::ThreadSafeModule(std::move(TheModule),
                                                   std::move(TheContext));
            EOE(TheJIT->addModule(std::move(TSM), RT));
            InitializeModuleAndPassManagers();

            // Search the JIT for the __anon_expr symbol.
            auto ExprSymbol = EOE(TheJIT->lookup("__anon_expr"));
            if (ExprSymbol.getFlags().hasError()) {
                fprintf(stderr, "Error occurs when looking up ExprSymbol");
                abort();
            }
            if (!ExprSymbol.getFlags().isCallable()) {
                fprintf(stderr, "Function not callable");
                abort();
            }

            // Get the symbol's address and cast it to the right type (takes no
            // arguments, returns a double) so we can call it as a native
            // function.
            double (*FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
            fprintf(stderr, "Evaluated to %f\n", FP());

            // Delete the anonymous expression module from the JIT.
            EOE(RT->remove());
        }
    } else {
        getNextToken();
    }
}

/// top ::= definition | external | expression | ';'
static void MainLoop() {
    while (true) {
        fprintf(stderr, "ready> ");
        switch (CurTok) {
        case tok_eof:
            return;
        case ';': // ignore top-level semicolons.
            getNextToken();
            break;
        case tok_def:
            HandleDefinition();
            break;
        case tok_extern:
            HandleExtern();
            break;
        default:
            HandleTopLevelExpression();
            break;
        }
    }
}

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT double putchard(double X) {
    fputc((char)X, stderr);
    return 0;
}

extern "C" DLLEXPORT double printd(double X) {
    fprintf(stderr, "%f\n", X);
    return 0;
}

int main() {
    BinopPrecedence['='] = 2;
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 20;
    BinopPrecedence['*'] = 40;

    fprintf(stdout, "ready> ");
    getNextToken();

    TheJIT = EOE(orc::KaleidoscopeJIT::Create());
    InitializeModuleAndPassManagers();
    MainLoop();
    TheModule->print(errs(), nullptr);

    auto TargetTriple = LLVMGetDefaultTargetTriple();

    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);
    if (!Target) {
        errs() << Error;
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;

    auto TargetMachine = Target->createTargetMachine(
        TargetTriple, CPU, Features, opt, Reloc::PIC_);

    TheModule->setDataLayout(TargetMachine->createDataLayout());
    TheModule->setTargetTriple(TargetTriple);

    auto FileName = "output.o";
    std::error_code EC;
    raw_fd_ostream dest(FileName, EC, llvm::sys::fs::OF_None);

    if (EC) {
        errs() << "Couldnt open file : " << EC.message();
        return 1;
    }

    llvm::legacy::PassManager pass;

    auto FileType = CodeGenFileType::ObjectFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        errs() << "TargetMachine cant emit a file of this type";
        return 1;
    }

    pass.run(*TheModule);
    dest.flush();

    return 0;
}
