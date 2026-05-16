#ifndef MAMBO_SEMA_H
#define MAMBO_SEMA_H

#include "mambo/Basic/Ast.h"
#include "mambo/Basic/Diagnostic.h"
#include "mambo/Sema/Scope.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

namespace mambo {

class Sema {
private:
  DiagnosticEngine &DE;
  LispScope *CurrentScope;

public:
  Sema(DiagnosticEngine &DE) : DE(DE) {}

  std::unique_ptr<VarDefExpr> actOnGlobalVarDef(llvm::StringRef VarName,
                                                std::unique_ptr<SExpr> Val,
                                                llvm::SMLoc Loc);

  void enterScope(SExpr &SExpr);
  void leaveScope();
};

class EnterDeclScope {
private:
  Sema &S;

public:
  EnterDeclScope(Sema &Sema, SExpr &SExpr) : S(Sema) { S.enterScope(SExpr); }

  ~EnterDeclScope() { S.leaveScope(); }
};

} // namespace mambo

#endif
