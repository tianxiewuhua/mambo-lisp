#ifndef MAMBO_PARSER_H
#define MAMBO_PARSER_H

#include "mambo/Basic/Ast.h"
#include "mambo/Lex/Lexer.h"
#include <memory>

namespace mambo {

class SExprParser {
private:
  Lexer &Lex;
  Token CurTok;

  std::unique_ptr<NumberExpr> parseNumber();
  std::unique_ptr<StringExpr> parseString();
  std::unique_ptr<FunctionPrototype> parseFunctionPrototype();
  std::unique_ptr<FunctionDefineExpr> parseFunctionDefine();
  std::unique_ptr<FunctionCallExpr> parseFunctionCall();

public:
  SExprParser(Lexer &Lex);

  void nextToken();
  std::unique_ptr<SExpr> parseSExpr();
  std::unique_ptr<TransitionUnit> parseTransitionUnit();
};

} // namespace mambo

#endif
