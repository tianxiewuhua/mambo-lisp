#include "mambo/Sema/Sema.h"
#include "mambo/Basic/Ast.h"
#include "mambo/Basic/Diagnostic.h"
#include "mambo/Sema/Scope.h"
#include <memory>
#include <utility>
#include <vector>

using namespace mambo;

std::unique_ptr<VarDefExpr> Sema::actOnDynamicVarDef(llvm::StringRef VarName,
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

std::unique_ptr<LetBindingsExpr>
Sema::actOnLetBindings(std::unique_ptr<LetBindingsExpr> LetBinding,
                       std::vector<VarDefExpr *> Vars) {

  std::vector<std::unique_ptr<VarDefExpr>> VarBindings;
  for (VarDefExpr *Var : Vars) {
    if (CurrentScope->insert(Var)) {
      DE.report(Var->getLocation(), diagnostic::err_symbol_duplicated,
                Var->getName());
      return nullptr;
    }
    VarBindings.push_back(std::unique_ptr<VarDefExpr>(Var));
  }

  LetBinding->setVarBindings(std::move(VarBindings));
  return LetBinding;
}

std::unique_ptr<FunctionDefineExpr>
Sema::actOnFunctionDef(std::unique_ptr<FunctionPrototype> Proto,
                       std::unique_ptr<SExpr> Body, llvm::SMLoc Loc) {
  // TODO check function name dumplicated in cuurent scope

  return std::make_unique<FunctionDefineExpr>(std::move(Proto), std::move(Body),
                                              Loc);
}

void Sema::enterScope(SExpr *S) { CurrentScope = new LispScope(CurrentScope); }

void Sema::leaveScope() {
  LispScope *ParentScope = CurrentScope->Parent;
  delete CurrentScope;
  CurrentScope = ParentScope;
}
