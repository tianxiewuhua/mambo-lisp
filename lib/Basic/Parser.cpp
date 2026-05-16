#include "mambo/Basic/Parser.h"
#include "mambo/Basic/Ast.h"
#include "mambo/Basic/TokenKinds.h"
#include "mambo/CodeGen/CodeGen.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

using namespace mambo;

SExprParser::SExprParser(Lexer &Lex, Sema &SemaAction)
    : Lex(Lex), SemaAction(SemaAction) {
  nextToken();
}

void SExprParser::nextToken() { Lex.read(CurTok); }

std::unique_ptr<NumberExpr> SExprParser::parseNumber() {
  if (CurTok.getKind() != tok::number_literal) {
    // TODO: error
    return nullptr;
  }

  double Val = strtod(CurTok.getText().str().c_str(), NULL);
  nextToken();
  return std::make_unique<NumberExpr>(Val, Lex.getLoc());
}

std::unique_ptr<StringExpr> SExprParser::parseString() {
  if (CurTok.getKind() != tok::string_literal) {
    // TODO: error
    return nullptr;
  }
  std::string Val = CurTok.getText().str();

  nextToken();
  return std::make_unique<StringExpr>(Val, Lex.getLoc());
}

std::unique_ptr<FunctionDefineExpr> SExprParser::parseFunctionDefine() {
  if (CurTok.getKind() != tok::kw_defun) {
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

  return std::make_unique<FunctionDefineExpr>(std::move(Proto), std::move(Body),
                                              Lex.getLoc());
}

std::unique_ptr<FunctionPrototype> SExprParser::parseFunctionPrototype() {
  if (CurTok.getKind() != tok::symbol) {
    // TODO: error
    return nullptr;
  }
  llvm::StringRef NameRef = CurTok.getText();

  nextToken();
  if (CurTok.getKind() != tok::l_paren) {
    // TODO: error
    return nullptr;
  }

  nextToken();
  std::vector<std::string> params;
  while (CurTok.getKind() == tok::symbol) {
    params.push_back(CurTok.getText().str());
    nextToken();
  }

  if (CurTok.getKind() != tok::r_paren) {
    // TODO: error
    return nullptr;
  }

  nextToken();
  return std::make_unique<FunctionPrototype>(NameRef.str(), params);
}

std::unique_ptr<FunctionCallExpr> SExprParser::parseFunctionCall() {
  if (CurTok.getKind() != tok::symbol) {
    // TODO: error
    return nullptr;
  }

  llvm::StringRef CalleeRef = CurTok.getText();
  nextToken();

  std::vector<std::unique_ptr<SExpr>> params;
  while (CurTok.getKind() != tok::r_paren) {
    std::unique_ptr<SExpr> param = parseSExpr();
    if (!param) {
      // TODO: error
      return nullptr;
    }

    params.push_back(std::move(param));
  }

  nextToken();
  return std::make_unique<FunctionCallExpr>(CalleeRef.str(), std::move(params),
                                            Lex.getLoc());
}

std::unique_ptr<VarDefExpr> SExprParser::parseGlobalVarDef() {
  if (CurTok.getKind() != tok::kw_defvar) {
    // TODO: error
    return nullptr;
  }

  nextToken();
  llvm::StringRef VarName = CurTok.getText();
  nextToken();
  std::unique_ptr<SExpr> Val = parseSExpr();

  return SemaAction.actOnGlobalVarDef(VarName, std::move(Val), Lex.getLoc());
}

std::unique_ptr<SExpr> SExprParser::parseSExpr() {
  if (CurTok.getKind() == tok::eof) {
    // end of file
    return nullptr;
  }

  if (CurTok.getKind() == tok::number_literal) {
    return parseNumber();
  }

  if (CurTok.getKind() == tok::string_literal) {
    return parseString();
  }

  if (CurTok.getKind() != tok::l_paren) {
    // TODO: error
    return nullptr;
  }

  nextToken();

  switch (CurTok.getKind()) {
  case tok::kw_defun:
    return parseFunctionDefine();
    break;
  case tok::symbol:
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
