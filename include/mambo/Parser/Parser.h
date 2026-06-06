#ifndef MAMBO_PARSER_H
#define MAMBO_PARSER_H

#include "mambo/Basic/Ast.h"
#include "mambo/Basic/TokenKinds.h"
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
  VarDefExpr *parseLexicalVarDef();
  std::unique_ptr<LetBindingsExpr> parseLetBinding();
  std::unique_ptr<VarDefExpr> parseDynamicVarDef();

public:
  SExprParser(Lexer &Lex, Sema &SemaAction);

  void nextToken();
  bool consume(tok::TokenKind ExpectedKind);
  bool expect(tok::TokenKind ExpectedKind);
  std::unique_ptr<SExpr> parseSExpr();
  std::unique_ptr<TransitionUnit> parseTransitionUnit();
};

} // namespace mambo

#endif
