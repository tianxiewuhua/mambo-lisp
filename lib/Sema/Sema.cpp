#include "mambo/Sema/Sema.h"
#include "mambo/Basic/Ast.h"
#include "mambo/Basic/Diagnostic.h"
#include "mambo/Sema/Scope.h"

using namespace mambo;

std::unique_ptr<VarDefExpr> Sema::actOnGlobalVarDef(llvm::StringRef VarName,
                                                    std::unique_ptr<SExpr> Val,
                                                    llvm::SMLoc Loc) {
  std::unique_ptr<VarDefExpr> VarDef = std::make_unique<VarDefExpr>(
      VarDefExpr::Dynamic, VarName.str(), std::move(Val), Loc);

  if (!CurrentScope->insert(VarDef.get())) {
    DE.report(Loc, diagnostic::err_symbol_duplicated, VarName);
    return nullptr;
  }

  return VarDef;
}

void Sema::enterScope(SExpr &S) { CurrentScope = new LispScope(CurrentScope); }

void Sema::leaveScope() {
  LispScope *ParentScope = CurrentScope->Parent;
  delete CurrentScope;
  CurrentScope = ParentScope;
}
