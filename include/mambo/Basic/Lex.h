#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"

class Lexer;

class Token {
  friend class Lexer;

public:
  enum TokenKind : unsigned short {
    eoi,
    unknown,
    symbol,
    number,
    lp,
    rp,
    str,
    defun,
  };

private:
  TokenKind Kind;
  llvm::StringRef Text;

public:
  Token::TokenKind getKind();
  llvm::StringRef getText();
};

class Lexer {
private:
  const char *BufferStart;
  const char *BufferPtr;

public:
  Lexer(const llvm::StringRef &Buffer) {
    BufferStart = Buffer.begin();
    BufferPtr = BufferStart;
  }

  void next(Token &tok);
};
#endif
