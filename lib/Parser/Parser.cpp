#include "mambo/Parser/Parser.h"
#include "mambo/Basic/Ast.h"
#include "mambo/Basic/TokenKinds.h"
#include "mambo/Sema/Sema.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

using namespace mambo;

SExprParser::SExprParser(Lexer &Lex, Sema &SemaAction)
    : Lex(Lex), SemaAction(SemaAction) {
  nextToken();
}

void SExprParser::nextToken() { Lex.read(CurTok); }

// return true if current token is not expected!
bool SExprParser::consume(tok::TokenKind ExpectedKind) {
  if (CurTok.getKind() == ExpectedKind) {
    nextToken();
    return false;
  }

  return true;
}

// return true if current token is not expected!
bool SExprParser::expect(tok::TokenKind ExpectedKind) {
  if (CurTok.getKind() == ExpectedKind) {
    return false;
  }

  return true;
}

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

std::unique_ptr<FunctionDefineExpr> SExprParser::parseFunctionDefine() {
  if (CurTok.getKind() != tok::so_defun) {
    // TODO: error
    return nullptr;
  }
  llvm::SMLoc CurLoc = Lex.getLoc();

  nextToken();
  std::unique_ptr<FunctionPrototype> Proto = parseFunctionPrototype();
  if (!Proto) {
    return nullptr;
  }

  std::unique_ptr<SExpr> Body = parseSExpr();
  if (!Body) {
    return nullptr;
  }

  // TODO Enter Scope

  return SemaAction.actOnFunctionDef(std::move(Proto), std::move(Body), CurLoc);
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

VarDefExpr *SExprParser::parseLexicalVarDef() {
  if (!expect(tok::symbol)) {
    llvm::StringRef VarName = CurTok.getText();
    llvm::SMLoc VarLoc = Lex.getLoc();

    nextToken();
    return new VarDefExpr(VarDefExpr::Lexical, VarName, nullptr, VarLoc);
  }

  if (consume(tok::l_paren)) {
    // TODO: ERROR
    return nullptr;
  }

  if (expect(tok::symbol)) {
    // TODO: ERROR
    return nullptr;
  }

  llvm::StringRef VarName = CurTok.getText();
  llvm::SMLoc VarLoc = Lex.getLoc();
  nextToken();
  std::unique_ptr<SExpr> Val = parseSExpr();

  if (consume(tok::r_paren)) {
    // TDOO Error
    return nullptr;
  }

  return new VarDefExpr(VarDefExpr::Lexical, VarName, std::move(Val), VarLoc);
}

std::unique_ptr<LetBindingsExpr> SExprParser::parseLetBinding() {
  if (consume(tok::so_let)) {
    // TODO error
    return nullptr;
  }
  llvm::SMLoc CurLoc = Lex.getLoc();

  if (consume(tok::l_paren)) {
    // TODO error
    return nullptr;
  }

  std::vector<VarDefExpr *> VarBindings;
  while (consume(tok::r_paren)) {
    VarDefExpr *VarBinding = parseLexicalVarDef();
    if (!VarBinding) {
      return nullptr;
    }

    VarBindings.push_back(VarBinding);
    if (consume(tok::r_paren)) {
      // TODO report error
      return nullptr;
    }
  }

  if (VarBindings.empty()) {
    // TODO error
  }

  std::unique_ptr<LetBindingsExpr> LetBinding =
      std::make_unique<LetBindingsExpr>(CurLoc);

  EnterDeclScope EDS(SemaAction, LetBinding.get());
  LetBinding = SemaAction.actOnLetBindings(std::move(LetBinding), VarBindings);

  std::vector<std::unique_ptr<SExpr>> LetBody;
  while (consume(tok::r_paren)) {
    std::unique_ptr<SExpr> Expr = parseSExpr();
    if (!Expr) {
      // TODO report error
      return nullptr;
    }

    LetBody.push_back(std::move(Expr));
  }

  if (LetBody.empty()) {
    // TODO report ERROR
    return nullptr;
  }
  LetBinding->setLetExprs(std::move(LetBody));

  return LetBinding;
}

std::unique_ptr<VarDefExpr> SExprParser::parseDynamicVarDef() {
  llvm::SMLoc CurLoc = Lex.getLoc();
  if (CurTok.getKind() != tok::so_defvar) {
    // TODO: error
    return nullptr;
  }

  nextToken();
  llvm::StringRef VarName = CurTok.getText();
  nextToken();
  std::unique_ptr<SExpr> Val = parseSExpr();

  return SemaAction.actOnDynamicVarDef(VarName, std::move(Val), CurLoc);
}

std::unique_ptr<IfExpr> SExprParser::parseIf() {
  llvm::SMLoc CurLoc = Lex.getLoc();

  if (consume(tok::so_if)) {
    // TODO: error
    return nullptr;
  }

  std::unique_ptr<SExpr> TestExpr = parseSExpr();
  std::unique_ptr<SExpr> ThenExpr = parseSExpr();
  if (expect(tok::r_paren)) {
    std::unique_ptr<SExpr> ElseExpr = parseSExpr();
    return std::make_unique<IfExpr>(std::move(TestExpr), std::move(ThenExpr),
                                    std::move(ElseExpr), CurLoc);
  }

  return std::make_unique<IfExpr>(std::move(TestExpr), std::move(ThenExpr),
                                  nullptr, CurLoc);
}

std::unique_ptr<SExpr> SExprParser::parseSExpr() {
  if (!expect(tok::eof)) {
    // end of file
    return nullptr;
  }

  if (!expect(tok::number_literal))
    return parseNumber();

  if (!expect(tok::string_literal))
    return parseString();

  if (!expect(tok::symbol)) {
    // TODO 获取符号本身
  }

  if (consume(tok::l_paren)) {
    // TODO: error
    return nullptr;
  }

  switch (CurTok.getKind()) {
  case tok::so_defun:
    return parseFunctionDefine();
    break;
  case tok::symbol:
    return parseFunctionCall();
  case tok::so_defvar:
    return parseDynamicVarDef();
  case tok::so_let:
    return parseLetBinding();
  case tok::so_if:
    return parseIf();
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
