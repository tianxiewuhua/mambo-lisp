#ifndef MAMBO_BASIC_AST_H
#define MAMBO_BASIC_AST_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

class SExpr {
public:
  enum SExprKind {
    SK_VarDef,
    SK_LetBindings,
    SK_If,
    SK_FunctionCall,
    SK_FunctionDef,
    SK_Number,
    SK_Var,
    SK_String
  };

private:
  const SExprKind Kind;
  llvm::SMLoc Loc;

public:
  SExpr(SExprKind Kind, llvm::SMLoc Loc) : Kind(Kind), Loc(Loc) {}
  virtual ~SExpr() = default;

  SExprKind getKind() const { return Kind; }
  llvm::SMLoc getLocation() { return Loc; }
};

class TransitionUnit {
private:
  std::vector<std::unique_ptr<SExpr>> SExprs;

public:
  TransitionUnit(std::vector<std::unique_ptr<SExpr>> SExprs)
      : SExprs(std::move(SExprs)) {}

  std::vector<std::unique_ptr<SExpr>> &getExprs() { return SExprs; };
};

class VarDefExpr : public SExpr {
public:
  enum VarDefKind { Dynamic, Lexical };

private:
  const VarDefKind Kind;

  llvm::StringRef Name;
  std::unique_ptr<SExpr> Val;

public:
  VarDefExpr(VarDefKind VarDefKind, llvm::StringRef Name,
             std::unique_ptr<SExpr> Val, llvm::SMLoc Loc)
      : SExpr(SK_VarDef, Loc), Kind(VarDefKind), Name(Name),
        Val(std::move(Val)) {}

  static bool classof(const SExpr *S) { return S->getKind() == SK_VarDef; }

  VarDefKind getKind() { return Kind; }
  llvm::StringRef getName() { return Name; }
};

class LetBindingsExpr : public SExpr {
private:
  std::vector<std::unique_ptr<VarDefExpr>> VarBindings;
  std::vector<std::unique_ptr<SExpr>> LetExprs;

public:
  LetBindingsExpr(llvm::SMLoc Loc) : SExpr(SK_LetBindings, Loc) {}

  static bool classof(const SExpr *S) { return S->getKind() == SK_LetBindings; }

  void setVarBindings(std::vector<std::unique_ptr<VarDefExpr>> VarBindings) {
    this->VarBindings = std::move(VarBindings);
  }

  void setLetExprs(std::vector<std::unique_ptr<SExpr>> LetExprs) {
    this->LetExprs = std::move(LetExprs);
  }
};

class IfExpr : public SExpr {
private:
  std::unique_ptr<SExpr> Cond, Then, Else;
  // TODO else form

public:
  IfExpr(std::unique_ptr<SExpr> Cond, std::unique_ptr<SExpr> Then,
         std::unique_ptr<SExpr> Else, llvm::SMLoc Loc)
      : SExpr(SK_If, Loc), Cond(std::move(Cond)), Then(std::move(Then)),
        Else(std::move(Else)) {}

  static bool classof(const SExpr *S) { return S->getKind() == SK_If; }

  SExpr *getCond() { return Cond.get(); }
  SExpr *getThen() { return Then.get(); }
  SExpr *getElse() { return Else.get(); }
};

class FunctionCallExpr : public SExpr {
private:
  std::string Callee;
  std::vector<std::unique_ptr<SExpr>> Args;

public:
  FunctionCallExpr(const std::string Callee,
                   std::vector<std::unique_ptr<SExpr>> Args, llvm::SMLoc Loc)
      : SExpr(SK_FunctionCall, Loc), Callee(Callee), Args(std::move(Args)) {};

  static bool classof(const SExpr *S) {
    return S->getKind() == SK_FunctionCall;
  }

  std::string getCallee() { return Callee; }
  std::vector<std::unique_ptr<SExpr>> &getArgs() { return Args; }
};

class NumberExpr : public SExpr {
private:
  double Val;

public:
  NumberExpr(double Val, llvm::SMLoc Loc) : SExpr(SK_Number, Loc), Val(Val) {}

  static bool classof(const SExpr *S) { return S->getKind() == SK_Number; }

  double getVal() { return Val; }
};

class StringExpr : public SExpr {
private:
  std::string Val;

public:
  StringExpr(const std::string Val, llvm::SMLoc Loc)
      : SExpr(SK_String, Loc), Val(Val) {}

  static bool classof(const SExpr *S) { return S->getKind() == SK_String; }

  std::string getVal() { return Val; }
};

class VarExpr : public SExpr {
private:
  std::string Name;

public:
  VarExpr(const std::string Name, llvm::SMLoc Loc)
      : SExpr(SK_Var, Loc), Name(Name) {}

  static bool classof(const SExpr *S) { return S->getKind() == SK_Var; }
};

class FunctionPrototype {
private:
  std::string Name;
  std::vector<std::string> Args;

public:
  FunctionPrototype(std::string Name, std::vector<std::string> Args)
      : Name(Name), Args(std::move(Args)) {};

  std::string getName() { return Name; }
  std::vector<std::string> &getArgs() { return Args; }
};

class FunctionDefineExpr : public SExpr {
private:
  std::unique_ptr<FunctionPrototype> Proto;
  std::unique_ptr<SExpr> Body;

public:
  FunctionDefineExpr(std::unique_ptr<FunctionPrototype> Proto,
                     std::unique_ptr<SExpr> Body, llvm::SMLoc Loc)
      : SExpr(SK_FunctionDef, Loc), Proto(std::move(Proto)),
        Body(std::move(Body)) {};

  FunctionPrototype *getProto() { return Proto.get(); }
  SExpr *getBody() { return Body.get(); }

  static bool classof(const SExpr *S) { return S->getKind() == SK_FunctionDef; }
};

#endif
