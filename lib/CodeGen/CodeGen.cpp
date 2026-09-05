#include "mambo/CodeGen/CodeGen.h"
#include "mambo/Basic/Ast.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include <vector>

using namespace mambo::CodeGen;

std::vector<llvm::Value *>
CodeGenFunctions::EmitTransitionUnit(TransitionUnit *T) {
  std::vector<llvm::Value *> Vals;
  std::vector<std::unique_ptr<SExpr>> &Exprs = T->getExprs();

  for (std::unique_ptr<SExpr> &SExpr : Exprs) {
    llvm::Value *Val = EmitSExpr(SExpr.get());
    if (!Val) {
      // TODO error
    }

    Vals.push_back(Val);
  }

  return Vals;
}

llvm::Value *CodeGenFunctions::EmitSExpr(SExpr *S) {
  if (NumberExpr *N = llvm::dyn_cast<NumberExpr>(S)) {
    return EmitNumerExpr(N);
  }

  if (StringExpr *Str = llvm::dyn_cast<StringExpr>(S)) {
    return EmitStringExpr(Str);
  }

  if (VarExpr *V = llvm::dyn_cast<VarExpr>(S)) {
    return EmitVarExpr(V);
  }

  if (VarDefExpr *VD = llvm::dyn_cast<VarDefExpr>(S)) {
    return EmitVarDefExpr(VD);
  }

  if (FunctionDefineExpr *FD = llvm::dyn_cast<FunctionDefineExpr>(S)) {
    return EmitFunctionDefExpr(FD);
  }

  if (FunctionCallExpr *FC = llvm::dyn_cast<FunctionCallExpr>(S)) {
    return EmitFunctionCallExpr(FC);
  }

  if (IfExpr *IF = llvm::dyn_cast<IfExpr>(S)) {
    return EmitIfExpr(IF);
  }

  // TODO: ERROR
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

llvm::Function *CodeGenFunctions::EmitFunctionProto(FunctionPrototype *FP) {
  std::vector<std::string> &Args = FP->getArgs();
  llvm::Function *F = produceFunction(FP->getName(), Args.size(), M);
  unsigned Idx = 0;
  for (llvm::Argument &Arg : F->args()) {
    Arg.setName(Args[Idx++]);
  }

  return F;
}

llvm::Value *CodeGenFunctions::EmitFunctionDefExpr(FunctionDefineExpr *FD) {
  // TODO:
  // 假设目前所有函数的参数类型和返回值都是double类型，后序再实现动态类型
  // 假设目前函数里只有一行S表达式，简化实现
  FunctionPrototype *FP = FD->getProto();
  llvm::Function *F = EmitFunctionProto(FP);
  if (!F) {
    // TODO errro
    return nullptr;
  }

  llvm::BasicBlock *EntryB =
      llvm::BasicBlock::Create(M.getContext(), "entry", F);
  Builder.SetInsertPoint(EntryB);

  llvm::Value *RetValue = EmitSExpr(FD->getBody());
  if (!RetValue) {
    // TODO error
    F->eraseFromParent();
    return nullptr;
  }

  Builder.CreateRet(RetValue);
  llvm::verifyFunction(*F);

  // TODO: 优化IR Pass
  return F;
}

llvm::Value *CodeGenFunctions::EmitFunctionCallExpr(FunctionCallExpr *FC) {
  std::string Callee = FC->getCallee();
  llvm::Function *F = M.getFunction(Callee);
  if (!F) {
    F = produceFunction(Callee, FC->getArgs().size(), M);
  }

  std::vector<llvm::Value *> ArgsV;
  for (std::unique_ptr<SExpr> &Arg : FC->getArgs()) {
    llvm::Value *ArgV = EmitSExpr(Arg.get());
    if (!ArgV) {
      // TODO error
      return nullptr;
    }

    ArgsV.push_back(ArgV);
  }

  return Builder.CreateCall(F, ArgsV, "call_" + Callee);
}

llvm::Value *CodeGenFunctions::EmitNumerExpr(NumberExpr *N) {
  return llvm::ConstantFP::get(M.getContext(), llvm::APFloat(N->getVal()));
}

llvm::Value *CodeGenFunctions::EmitStringExpr(StringExpr *Str) {
  // TODO
}
llvm::Value *CodeGenFunctions::EmitVarExpr(VarExpr *V) {
  // TODO
}
llvm::Value *CodeGenFunctions::EmitVarDefExpr(VarDefExpr *VD) {
  // TODO
}
llvm::Value *EmitLetBindingsExpr(LetBindingsExpr *LB) {
  // TODO
}
llvm::Value *CodeGenFunctions::EmitIfExpr(IfExpr *IF) {
  // TODO
}
