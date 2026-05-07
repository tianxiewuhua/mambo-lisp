#include "mambo/Basic/TokenKinds.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

#ifndef MAMBO_LEXER_TOKEN
#define MAMBO_LEXER_TOKEN

namespace mambo {

class Lexer;

class Token {
  friend class Lexer;

private:
  tok::TokenKind Kind;

  const char *DataPtr;
  size_t Length;

public:
  tok::TokenKind getKind() const { return Kind; }
  llvm::StringRef getText() const { return llvm::StringRef(DataPtr, Length); }

  const char *getName() const { return tok::getTokenName(Kind); }
  llvm::SMLoc getLocation() const {
    return llvm::SMLoc::getFromPointer(DataPtr);
  }
};
} // namespace mambo

#endif
