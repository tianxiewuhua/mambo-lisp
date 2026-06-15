#include "mambo/Basic/TokenKinds.h"
#include <llvm/Support/ErrorHandling.h>

using namespace mambo;

static const char *const TokNames[] = {
#define TOK(ID) #ID,
#define SPECIALOP(ID, FLAG) #ID,
#include "mambo/Basic/TokenKinds.def"
    nullptr};

const char *tok::getTokenName(TokenKind Kind) {
  if (Kind < tok::NUM_TOKENS) {
    return TokNames[Kind];
  }

  llvm_unreachable("unknown TokenKind");
  return nullptr;
}

const char *tok::getSpecialOperatorSpelling(TokenKind Kind) {
  switch (Kind) {
#define SPECIALOP(ID, FLAG)                                                    \
  case so_##ID:                                                                \
    return #ID;
#include "mambo/Basic/TokenKinds.def"
  default:
    break;
  }

  return nullptr;
}
