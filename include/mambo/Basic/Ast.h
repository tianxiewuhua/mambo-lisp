#ifndef AST_H
#define AST_H

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/SMLoc.h"

class SExpr {
private:
  llvm::SMLoc Loc;

public:
  SExpr(llvm::SMLoc Loc) : Loc(Loc) {}
  virtual ~SExpr() = default;

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) = 0;
  llvm::SMLoc getLocation() { return Loc; }
};

class TransitionUnit {
private:
  std::vector<std::unique_ptr<SExpr>> SExprs;

public:
  TransitionUnit(std::vector<std::unique_ptr<SExpr>> SExprs)
      : SExprs(std::move(SExprs)) {}

  std::vector<llvm::Value *> codegen(llvm::Module &M, llvm::IRBuilder<> &B);
};

class VarDefExpr : public SExpr {
public:
  enum VarDefKind { Dynamic, Lexical };

private:
  const VarDefKind Kind;

  llvm::StringRef Name;
  std::unique_ptr<SExpr> Val;

public:
  VarDefExpr(VarDefKind Kind, llvm::StringRef Name, std::unique_ptr<SExpr> Val,
             llvm::SMLoc Loc)
      : SExpr(Loc), Kind(Kind), Name(Name), Val(std::move(Val)) {}

  VarDefKind getKind() { return Kind; }
  llvm::StringRef getName() { return Name; }

  llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class LetBindingsExpr : public SExpr {
private:
  std::vector<std::unique_ptr<VarDefExpr>> VarBindings;
  std::vector<std::unique_ptr<SExpr>> LetExprs;

public:
  LetBindingsExpr(llvm::SMLoc Loc) : SExpr(Loc) {}

  void setVarBindings(std::vector<std::unique_ptr<VarDefExpr>> VarBindings) {
    this->VarBindings = std::move(VarBindings);
  }

  void setLetExprs(std::vector<std::unique_ptr<SExpr>> LetExprs) {
    this->LetExprs = std::move(LetExprs);
  }

  llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class FunctionCallExpr : public SExpr {
private:
  std::string Callee;
  std::vector<std::unique_ptr<SExpr>> Args;

public:
  FunctionCallExpr(const std::string Callee,
                   std::vector<std::unique_ptr<SExpr>> Args, llvm::SMLoc Loc)
      : SExpr(Loc), Callee(Callee), Args(std::move(Args)) {};

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class NumberExpr : public SExpr {
private:
  double Val;

public:
  NumberExpr(double Val, llvm::SMLoc Loc) : SExpr(Loc), Val(Val) {}

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class StringExpr : public SExpr {
private:
  std::string Val;

public:
  StringExpr(const std::string Val, llvm::SMLoc Loc) : SExpr(Loc), Val(Val) {}

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class VarExpr : public SExpr {
private:
  std::string Name;

public:
  VarExpr(const std::string Name, llvm::SMLoc Loc) : SExpr(Loc), Name(Name) {}

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class FunctionPrototype {
private:
  std::string Name;
  std::vector<std::string> Args;

public:
  FunctionPrototype(std::string Name, std::vector<std::string> Args)
      : Name(Name), Args(std::move(Args)) {};

  llvm::Function *codegen(llvm::Module &M, llvm::IRBuilder<> &B);
};

class FunctionDefineExpr : public SExpr {
private:
  std::unique_ptr<FunctionPrototype> Proto;
  std::unique_ptr<SExpr> Body;

public:
  FunctionDefineExpr(std::unique_ptr<FunctionPrototype> Proto,
                     std::unique_ptr<SExpr> Body, llvm::SMLoc Loc)
      : SExpr(Loc), Proto(std::move(Proto)), Body(std::move(Body)) {};

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

#endif
