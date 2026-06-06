#ifndef MAMBO_SEMA_H
#define MAMBO_SEMA_H

#include "mambo/Basic/Ast.h"
#include "mambo/Basic/Diagnostic.h"
#include "mambo/Sema/Scope.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"
#include <memory>

namespace mambo {

class Sema {
private:
  DiagnosticEngine &DE;
  LispScope *CurrentScope;

public:
  Sema(DiagnosticEngine &DE) : DE(DE) {}

  std::unique_ptr<VarDefExpr> actOnDynamicVarDef(llvm::StringRef VarName,
                                                 std::unique_ptr<SExpr> Val,
                                                 llvm::SMLoc Loc);
  std::unique_ptr<LetBindingsExpr>
  actOnLetBindings(std::unique_ptr<LetBindingsExpr> LetBinding,
                   std::vector<VarDefExpr *> Vars);
  std::unique_ptr<FunctionDefineExpr>
  actOnFunctionDef(std::unique_ptr<FunctionPrototype> Proto,
                   std::unique_ptr<SExpr> Body, llvm::SMLoc Loc);

  void enterScope(SExpr *SExpr);
  void leaveScope();
};

class EnterDeclScope {
private:
  Sema &S;

public:
  EnterDeclScope(Sema &Sema, SExpr *SExpr) : S(Sema) { S.enterScope(SExpr); }

  ~EnterDeclScope() { S.leaveScope(); }
};

} // namespace mambo

#endif
