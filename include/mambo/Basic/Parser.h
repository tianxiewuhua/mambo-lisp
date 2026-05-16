#ifndef MAMBO_PARSER_H
#define MAMBO_PARSER_H

#include "mambo/Basic/Ast.h"
#include "mambo/Lex/Lexer.h"
#include "mambo/Sema/Sema.h"
#include <memory>

namespace mambo {

class SExprParser {
private:
  Token CurTok;
  Lexer &Lex;
  Sema &SemaAction;

  std::unique_ptr<NumberExpr> parseNumber();
  std::unique_ptr<StringExpr> parseString();
  std::unique_ptr<FunctionPrototype> parseFunctionPrototype();
  std::unique_ptr<FunctionDefineExpr> parseFunctionDefine();
  std::unique_ptr<FunctionCallExpr> parseFunctionCall();
  std::unique_ptr<VarDefExpr> parseGlobalVarDef();

public:
  SExprParser(Lexer &Lex, Sema &SemaAction);

  void nextToken();
  std::unique_ptr<SExpr> parseSExpr();
  std::unique_ptr<TransitionUnit> parseTransitionUnit();
};

} // namespace mambo

#endif
