//For licensing terms, please read LICENSE.md in this repository.
//===----------------------------------------------------------------------===//
/*
  Hikari pass registration.
*/

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "Obfuscation/Obfuscation.h"

namespace llvm {
static llvm::RegisterStandardPasses RegisterHikari_Opt(
    llvm::PassManagerBuilder::EP_OptimizerLast,
    [](const llvm::PassManagerBuilder &Builder,
       llvm::legacy::PassManagerBase &PM) { PM.add(createObfuscationPass()); });

static llvm::RegisterStandardPasses RegisterHikari_NoOpt(
    llvm::PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const llvm::PassManagerBuilder &Builder,
       llvm::legacy::PassManagerBase &PM) { PM.add(createObfuscationPass()); });

} // namespace llvm

