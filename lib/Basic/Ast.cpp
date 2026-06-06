#include "mambo/Basic/Ast.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include <vector>

std::vector<llvm::Value *> TransitionUnit::codegen(llvm::Module &M,
                                                   llvm::IRBuilder<> &B) {
  std::vector<llvm::Value *> Vs;
  for (std::unique_ptr<SExpr> &SExpr : SExprs) {
    llvm::Value *V = SExpr->codegen(M, B);
    if (!V) {
      // TODO error
    }

    Vs.push_back(V);
  }

  return Vs;
}

llvm::Value *VarDefExpr::codegen(llvm::Module &M, llvm::IRBuilder<> &B) {
  // TODO
  return nullptr;
}

llvm::Value *LetBindingsExpr::codegen(llvm::Module &M, llvm::IRBuilder<> &B) {
  // TODO
  return nullptr;
}

llvm::Value *NumberExpr::codegen(llvm::Module &M, llvm::IRBuilder<> &B) {
  return llvm::ConstantFP::get(M.getContext(), llvm::APFloat(Val));
}

llvm::Value *StringExpr::codegen(llvm::Module &M, llvm::IRBuilder<> &B) {
  // TODO
  return nullptr;
}

static llvm::Function *produceFunction(std::string Name, size_t ArgsCount,
                                       llvm::Module &M) {
  std::vector<llvm::Type *> ParamTypes(ArgsCount,
                                       llvm::Type::getDoubleTy(M.getContext()));
  llvm::FunctionType *FunctionType = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(M.getContext()), ParamTypes, false);
  llvm::Function *F = llvm::Function::Create(
      FunctionType, llvm::Function::ExternalLinkage, Name, M);
  return F;
}

llvm::Value *FunctionCallExpr::codegen(llvm::Module &M, llvm::IRBuilder<> &B) {
  llvm::Function *F = M.getFunction(Callee);
  if (!F) {
    F = produceFunction(Callee, Args.size(), M);
  }

  std::vector<llvm::Value *> ArgsV;
  for (std::unique_ptr<SExpr> &Arg : Args) {
    llvm::Value *ArgV = Arg->codegen(M, B);
    if (!ArgV) {
      // TODO error
      return nullptr;
    }

    ArgsV.push_back(ArgV);
  }

  return B.CreateCall(F, ArgsV, "call_" + Callee);
}

llvm::Function *FunctionPrototype::codegen(llvm::Module &M,
                                           llvm::IRBuilder<> &B) {
  llvm::Function *F = produceFunction(Name, Args.size(), M);
  unsigned Idx = 0;
  for (llvm::Argument &Arg : F->args()) {
    Arg.setName(Args[Idx++]);
  }

  return F;
}

llvm::Value *FunctionDefineExpr::codegen(llvm::Module &M,
                                         llvm::IRBuilder<> &B) {
  // TODO:
  // 假设目前所有函数的参数类型和返回值都是double类型，后序再实现动态类型
  // 假设目前函数里只有一行S表达式，简化实现
  FunctionPrototype &P = *Proto;
  llvm::Function *F = P.codegen(M, B);
  if (!F) {
    // TODO errro
    return nullptr;
  }

  llvm::BasicBlock *EntryB =
      llvm::BasicBlock::Create(M.getContext(), "entry", F);
  B.SetInsertPoint(EntryB);

  llvm::Value *RetValue = Body->codegen(M, B);
  if (!RetValue) {
    // TODO error
    F->eraseFromParent();
    return nullptr;
  }

  B.CreateRet(RetValue);
  llvm::verifyFunction(*F);

  // TODO: 优化IR Pass
  return F;
}
