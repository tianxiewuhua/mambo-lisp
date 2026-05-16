#include "mambo/Sema/Scope.h"
#include "mambo/Basic/Ast.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

using namespace mambo;

bool LispScope::insert(VarDefExpr *VarDef) {
  return Symbols
      .insert(
          std::pair<llvm::StringRef, VarDefExpr *>(VarDef->getName(), VarDef))
      .second;
}

VarDefExpr *LispScope::lookup(llvm::StringRef Name) {
  LispScope *LookupScope = this;

  while (LookupScope) {
    llvm::StringMap<VarDefExpr *>::iterator it =
        LookupScope->Symbols.find(Name);
    if (it != Symbols.end()) {
      return it->second;
    }

    LookupScope = LookupScope->Parent;
  }

  return nullptr;
}
