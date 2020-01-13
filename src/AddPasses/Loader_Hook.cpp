#include "substrate.h"
#include <Obfuscation/Obfuscation.h>
#include <string>
#include <mach-o/dyld.h>

using namespace std;
void (*old_pmb)(void* dis,legacy::PassManagerBase &MPM);
Pass* (*old_get_LS)();
extern "C" Pass* _ZN4llvm21createLowerSwitchPassEv(){
  return old_get_LS();
}
static void new_pmb(void* dis,legacy::PassManagerBase &MPM){
  MPM.add(createObfuscationPass());
  old_pmb(dis,MPM);
}
static __attribute__((__constructor__)) void Inject(int argc, char* argv[]){
  char* executablePath=argv[0];
  errs()<<"Applying Apple Clang Hooks...\n";
  MSImageRef exeImagemage=MSGetImageByName(executablePath);
  MSHookFunction((void*)MSFindSymbol(exeImagemage,"__ZN4llvm18PassManagerBuilder25populateModulePassManagerERNS_6legacy15PassManagerBaseE"),(void*)new_pmb,(void**)&old_pmb);
  old_get_LS=(Pass* (*)())MSFindSymbol(exeImagemage,"__ZN4llvm15callDefaultCtorIN12_GLOBAL__N_111LowerSwitchEEEPNS_4PassEv");
}
