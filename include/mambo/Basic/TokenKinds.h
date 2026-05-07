#ifndef MAMBO_BASIC_TOKENKINDS_H
#define MAMBO_BASIC_TOKENKINDS_H

#include "llvm/Support/Compiler.h"

namespace mambo {
namespace tok {

enum TokenKind : unsigned short {
#define TOK(ID) ID,
#include "TokenKinds.def"
  NUM_TOKENS
};

const char *getTokenName(TokenKind Kind) LLVM_READNONE;
const char *getKeywordSpelling(TokenKind Kind) LLVM_READNONE;

} // namespace tok
} // namespace mambo

#endif
