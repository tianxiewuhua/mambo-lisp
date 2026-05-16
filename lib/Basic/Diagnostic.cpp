#include "mambo/Basic/Diagnostic.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SourceMgr.h"

using namespace mambo;

static const char *DiagnosticText[] = {
#define DIAG(ID, LEVEL, MSG) MSG,
#include "mambo/Basic/Diagnostic.def"
};

llvm::SourceMgr::DiagKind
DiagnosticEngine::getDiagnosticKind(unsigned int DiagID) {
  switch (DiagID) {
#define DIAG(ID, LEVEL, MSG)                                                   \
  case LEVEL:                                                                  \
    return llvm::SourceMgr::DK_##LEVEL;
  default:
    break;
  }

  llvm_unreachable("unknown DiagKind");
  return llvm::SourceMgr::DK_Error;
}

const char *DiagnosticEngine::getDiagnosticText(unsigned int DiagID) {
  return DiagnosticText[DiagID];
}
