#ifndef MAMBO_BASIC_DIAGNOSTIC_H
#define MAMBO_BASIC_DIAGNOSTIC_H

#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
namespace mambo {

namespace diagnostic {
enum {
#define DIAG(ID, LEVEL, MSG) ID,
#include "mambo/Basic/Diagnostic.def"
};
} // namespace diagnostic

class DiagnosticEngine {
private:
  llvm::SourceMgr &SrcMgr;
  unsigned int NumErrors;

  static llvm::SourceMgr::DiagKind getDiagnosticKind(unsigned int DiagID);
  static const char *getDiagnosticText(unsigned int DiagID);

public:
  DiagnosticEngine(llvm::SourceMgr &SrcMgr) : SrcMgr(SrcMgr), NumErrors(0) {}

  unsigned int numErrors() { return NumErrors; }

  template <typename... Args>
  void report(llvm::SMLoc Loc, unsigned int DiagID, Args &&...Arguments) {
    // TODO to be implemented

    llvm::SourceMgr::DiagKind Kind = getDiagnosticKind(DiagID);
    NumErrors += (Kind == llvm::SourceMgr::DK_Error);
  }
};
} // namespace mambo

#endif
