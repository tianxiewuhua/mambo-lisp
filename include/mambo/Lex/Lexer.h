#ifndef MAMBO_LEX_LEXER_H
#define MAMBO_LEX_LEXER_H

#include "mambo/Basic/Diagnostic.h"
#include "mambo/Basic/TokenKinds.h"
#include "mambo/Lex/Token.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/SMLoc.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/SourceMgr.h>

namespace mambo {

class KeywordsMgr {
private:
  llvm::StringMap<tok::TokenKind> SpecialOpsMap;

  void addSpecialOp(llvm::StringRef K, tok::TokenKind Kind);

public:
  void initSpecialOps();

  tok::TokenKind getSpecialOp(llvm::StringRef Name,
                              tok::TokenKind DefaultKind = tok::unknown) {
    llvm::StringMapIterBase<tok::TokenKind, false> Rel =
        SpecialOpsMap.find(Name);
    if (Rel != SpecialOpsMap.end()) {
      return Rel->second;
    }

    return DefaultKind;
  }
};

class Lexer {
private:
  const char *BufferStart;
  const char *BufferPtr;

  DiagnosticEngine &DE;
  KeywordsMgr KMgr;

  void validateDelimiter(size_t offset = 0);

public:
  Lexer(const llvm::SourceMgr &SrcMgr, DiagnosticEngine &DE) : DE(DE) {
    unsigned int FileID = SrcMgr.getMainFileID();
    BufferStart = BufferPtr =
        SrcMgr.getMemoryBuffer(FileID)->getBuffer().begin();

    KMgr.initSpecialOps();
  }

  void read(Token &tok);
  DiagnosticEngine &getDiagnostic() { return DE; }
  llvm::SMLoc getLoc() { return llvm::SMLoc::getFromPointer(BufferPtr); }
};

} // namespace mambo

#endif
