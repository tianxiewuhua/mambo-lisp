#include "mambo/Lex/Lexer.h"
#include "mambo/Basic/Diagnostic.h"
#include "mambo/Basic/TokenKinds.h"
#include <cctype>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Compiler.h>

using namespace mambo;

LLVM_READNONE static bool _is_function_punct(char _c) {
  switch (_c) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '?':
  case '!':
    return true;
  default:
    return false;
  }
}

void KeywordsMgr::initSpecialOps() {
#define SPECIALOP(ID, FLAGS) addSpecialOp(llvm::StringRef(#ID), tok::so_##ID);
#include "mambo/Basic/TokenKinds.def"
}

void KeywordsMgr::addSpecialOp(llvm::StringRef K, tok::TokenKind Kind) {
  SpecialOpsMap.insert(std::pair<llvm::StringRef, tok::TokenKind>(K, Kind));
}

void Lexer::validateDelimiter(size_t offset) {
  const char *checkPtr = BufferPtr + offset;

  if (!isspace(*checkPtr) || *checkPtr != ')') {
    DE.report(getLoc(), diagnostic::err_expect_space);
  }
}

void Lexer::read(Token &Tok) {
  while (*BufferPtr && isspace(*BufferPtr)) {
    ++BufferPtr;
  }

  if (!*BufferPtr) {
    Tok.Kind = tok::eof;
    return;
  }

  if (isalpha(*BufferPtr) || _is_function_punct(*BufferPtr)) {
    const char *BufferEnd = BufferPtr + 1;

    while (isalnum(*BufferEnd) || _is_function_punct(*BufferEnd)) {
      ++BufferEnd;
    }

    llvm::StringRef Name(BufferPtr, BufferEnd - BufferPtr);

    tok::TokenKind DefaultKind = tok::symbol;
    Tok.Kind = KMgr.getSpecialOp(Name, DefaultKind);
    Tok.DataPtr = BufferPtr;
    Tok.Length = BufferEnd - BufferPtr;

    BufferPtr = BufferEnd;

    validateDelimiter();
    return;
  }

  if (isdigit(*BufferPtr) || *BufferPtr == '.') {
    const char *BufferEnd = BufferPtr + 1;
    while (isdigit(*BufferEnd) || *BufferEnd == '.') {
      ++BufferEnd;
    }

    Tok.Kind = tok::number_literal;
    Tok.DataPtr = BufferPtr;
    Tok.Length = BufferEnd - BufferPtr;

    BufferPtr = BufferEnd;

    validateDelimiter();
    return;
  }

  if (*BufferPtr == '\"') {
    const char *BufferEnd = BufferPtr + 1;
    while (*BufferEnd != '\"' && *BufferPtr) {
      ++BufferEnd;
    }

    if (*BufferEnd != '\"') {
      DE.report(getLoc(), diagnostic::err_unterminated_string);
    }

    Tok.Kind = tok::string_literal;
    Tok.DataPtr = BufferPtr;
    Tok.Length = BufferEnd - BufferPtr;

    BufferPtr = BufferEnd;

    validateDelimiter();
    return;
  }

  switch (*BufferPtr) {
  case '(':
    Tok.Kind = tok::l_paren;
    Tok.DataPtr = BufferPtr;
    Tok.Length = 1;
    ++BufferPtr;
    break;
  case ')':
    Tok.Kind = tok::r_paren;
    Tok.DataPtr = BufferPtr;
    Tok.Length = 1;
    ++BufferPtr;
    break;
  default:
    Tok.Kind = tok::unknown;
    Tok.DataPtr = BufferPtr;
    Tok.Length = 1;
    ++BufferPtr;
    break;
  }
}
