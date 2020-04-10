#include "substrate.h"
#include <Obfuscation/Obfuscation.h>
#include <mach-o/dyld.h>
#include <string>

using namespace std;
void (*old_pmb)(void *dis, legacy::PassManagerBase &MPM);

static void new_pmb(void *dis, legacy::PassManagerBase &MPM) {
  MPM.add(createObfuscationPass());
  old_pmb(dis, MPM);
}
static __attribute__((__constructor__)) void Inject(int argc, char *argv[]) {
  char *executablePath = argv[0];
  errs() << "Applying Clang hook...\n";
  MSImageRef exeImagemage = MSGetImageByName(executablePath);
  MSHookFunction(
      (void *)MSFindSymbol(
          exeImagemage,
          "__ZN4llvm18PassManagerBuilder25populateModulePassManagerERNS_"
          "6legacy15PassManagerBaseE"),
      (void *)new_pmb, (void **)&old_pmb);
}
