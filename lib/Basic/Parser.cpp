#include "mambo/Basic/Parser.h"
#include "mambo/Basic/Ast.h"
#include "mambo/Basic/Lex.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

SExprParser::SExprParser(Lexer &Lex) : Lex(Lex) { nextToken(); }

void SExprParser::nextToken() { Lex.next(CurTok); }

std::unique_ptr<NumberExpr> SExprParser::parseNumber() {
  if (CurTok.getKind() != Token::number) {
    // TODO: error
    return nullptr;
  }

  double Val = strtod(CurTok.getText().str().c_str(), NULL);
  nextToken();
  return std::make_unique<NumberExpr>(Val);
}

std::unique_ptr<StringExpr> SExprParser::parseString() {
  if (CurTok.getKind() != Token::str) {
    // TODO: error
    return nullptr;
  }

  nextToken();
  return std::make_unique<StringExpr>(CurTok.getText().str());
}

std::unique_ptr<FunctionDefineExpr> SExprParser::parseFunctionDefine() {
  if (CurTok.getKind() != Token::defun) {
    // TODO: error
    return nullptr;
  }
  nextToken();

  std::unique_ptr<FunctionPrototype> Proto = parseFunctionPrototype();
  if (!Proto) {
    return nullptr;
  }

  std::unique_ptr<SExpr> Body = parseSExpr();
  if (!Body) {
    return nullptr;
  }

  return std::make_unique<FunctionDefineExpr>(std::move(Proto),
                                              std::move(Body));
}

std::unique_ptr<FunctionPrototype> SExprParser::parseFunctionPrototype() {
  if (CurTok.getKind() != Token::symbol) {
    // TODO: error
    return nullptr;
  }
  llvm::StringRef NameRef = CurTok.getText();

  nextToken();
  if (CurTok.getKind() != Token::lp) {
    // TODO: error
    return nullptr;
  }

  nextToken();
  std::vector<std::string> params;
  while (CurTok.getKind() == Token::symbol) {
    params.push_back(CurTok.getText().str());
    nextToken();
  }

  if (CurTok.getKind() != Token::rp) {
    // TODO: error
    return nullptr;
  }

  nextToken();
  return std::make_unique<FunctionPrototype>(NameRef.str(), params);
}

std::unique_ptr<FunctionCallExpr> SExprParser::parseFunctionCall() {
  if (CurTok.getKind() != Token::symbol) {
    // TODO: error
    return nullptr;
  }

  llvm::StringRef CalleeRef = CurTok.getText();
  nextToken();

  std::vector<std::unique_ptr<SExpr>> params;
  while (CurTok.getKind() != Token::rp) {
    std::unique_ptr<SExpr> param = parseSExpr();
    if (!param) {
      // TODO: error
      return nullptr;
    }

    params.push_back(std::move(param));
  }

  nextToken();
  return std::make_unique<FunctionCallExpr>(CalleeRef.str(), std::move(params));
}

std::unique_ptr<SExpr> SExprParser::parseSExpr() {
  if (CurTok.getKind() == Token::eoi) {
    // finish
    return nullptr;
  }

  if (CurTok.getKind() == Token::number) {
    return parseNumber();
  }

  if (CurTok.getKind() == Token::str) {
    return parseString();
  }

  if (CurTok.getKind() != Token::lp) {
    // TODO: error
    return nullptr;
  }

  nextToken();

  switch (CurTok.getKind()) {
  case Token::defun:
    return parseFunctionDefine();
    break;
  case Token::symbol:
    return parseFunctionCall();
  default:
    // TODO: error
    return nullptr;
  }
}

std::unique_ptr<TransitionUnit> SExprParser::parseTransitionUnit() {
  std::vector<std::unique_ptr<SExpr>> Sexprs;

  while (std::unique_ptr<SExpr> Sexpr = parseSExpr()) {
    Sexprs.push_back(std::move(Sexpr));
  }

  return std::make_unique<TransitionUnit>(std::move(Sexprs));
}
