#ifndef MAMBO_SEMA_SCOPE
#define MAMBO_SEMA_SCOPE

#include "mambo/Basic/Ast.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

namespace mambo {

class LispScope {
private:
  llvm::StringMap<VarDefExpr *> Symbols;

public:
  LispScope *Parent;

  LispScope(LispScope *Parent = nullptr) : Parent(Parent) {}

  bool insert(VarDefExpr *VarDef);
  VarDefExpr *lookup(llvm::StringRef Name);
};

} // namespace mambo

#endif
