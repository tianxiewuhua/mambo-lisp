#ifndef AST_H
#define AST_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include <string>
#include <vector>

class SExpr {
public:
  virtual ~SExpr() = default;
  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) = 0;
};

class TransitionUnit {
private:
  std::vector<std::unique_ptr<SExpr>> SExprs;

public:
  TransitionUnit(std::vector<std::unique_ptr<SExpr>> SExprs)
      : SExprs(std::move(SExprs)) {}

  std::vector<llvm::Value *> codegen(llvm::Module &M, llvm::IRBuilder<> &B);
};

class FunctionCallExpr : public SExpr {
private:
  std::string Callee;
  std::vector<std::unique_ptr<SExpr>> Args;

public:
  FunctionCallExpr(const std::string Callee,
                   std::vector<std::unique_ptr<SExpr>> Args)
      : Callee(Callee), Args(std::move(Args)) {};

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class NumberExpr : public SExpr {
private:
  double Val;

public:
  NumberExpr(double Val) : Val(Val) {}

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class StringExpr : public SExpr {
private:
  std::string Val;

public:
  StringExpr(const std::string Val) : Val(Val) {}

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

class VarExpr : public SExpr {
private:
  std::string Name;

public:
  VarExpr(const std::string Name) : Name(Name) {}

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
                     std::unique_ptr<SExpr> Body)
      : Proto(std::move(Proto)), Body(std::move(Body)) {};

  virtual llvm::Value *codegen(llvm::Module &M, llvm::IRBuilder<> &B) override;
};

#endif
