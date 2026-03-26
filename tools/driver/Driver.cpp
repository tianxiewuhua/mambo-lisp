#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/raw_os_ostream.h>
#include "mambo/Basic/Version.h"

int main(int argc_, const char **argv_) {
  llvm::InitLLVM X(argc_, argv_);

  llvm::outs() << "Mambo! " << mambo::getMamboVersion() << "\n";

  return 0;
}
