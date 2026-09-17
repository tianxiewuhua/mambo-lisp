#include "mambo/CodeGen/CodeGen.h"
#include "mambo/Basic/Ast.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
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

  llvm::BasicBlock *EntryBB =
      llvm::BasicBlock::Create(M.getContext(), "entry", F);
  Builder.SetInsertPoint(EntryBB);

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
  llvm::Value *CondVal = EmitSExpr(IF->getCond());
  CondVal = Builder.CreateFCmpONE(
      CondVal, llvm::ConstantFP::get(M.getContext(), llvm::APFloat(0.0)),
      "ifcond");

  // get current function
  llvm::Function *TheFunc = Builder.GetInsertBlock()->getParent();

  // 'if' S-Expression has three blocks in CFG
  // Then Block, Else Block, Continue Blcok(merge result in phi function)
  llvm::BasicBlock *ThenBB =
      llvm::BasicBlock::Create(M.getContext(), "then", TheFunc);
  llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(M.getContext(), "else");
  llvm::BasicBlock *ContBB =
      llvm::BasicBlock::Create(M.getContext(), "cont"); // todo: delete thefunc

  // * create br instruction
  // * IR: br i1 %condval , label %then, label %else
  Builder.CreateCondBr(CondVal, ThenBB, ElseBB);

  // Emit Then Block
  Builder.SetInsertPoint(ThenBB);
  llvm::Value *ThenVal = EmitSExpr(IF->getThen());
  if (!ThenVal) {
    // todo: thenVal maybe null
  }

  // - create an unconditional br to continue block;
  // - Note: all 'basic block' is required to be terminated with a 'control flow
  //         instruction' such as return or branch;
  Builder.CreateBr(ContBB);

  // Important: PHI需要绑定对应的value:block, 如果存在嵌套if-else,
  // 那么可能最终需要绑定的block可能不是之前的ThenBB,
  // 而是嵌套的if-continue-block 所以需要重新获取最后insert的BasicBlock
  ThenBB = Builder.GetInsertBlock();

  // Emit Else Block
  // 因为在ThenBlock codegen时，可能会产生其他的Block(比如嵌套if-else),
  // 为了保持可读性，ElseBlock/ContinueBlock在Create时不绑定Function，codegen时再通过Function->insert和Function绑定
  TheFunc->insert(TheFunc->end(), ElseBB);
  Builder.SetInsertPoint(ElseBB);
  llvm::Value *ElseVal = EmitSExpr(IF->getElse());
  if (!ElseVal) {
    // todo: ElseVal maybe null
  }

  // create an unconditional br to continue block for else;
  Builder.CreateBr(ContBB);
  // same to ThenBB
  ElseBB = Builder.GetInsertBlock();

  // Emit Continue Block
  TheFunc->insert(TheFunc->end(), ContBB);
  Builder.SetInsertPoint(ContBB);

  // PHI ex: %if_rel = phi double [ %then_value, %then ], [ %else_value, %else ]
  llvm::PHINode *PN =
      Builder.CreatePHI(llvm::Type::getDoubleTy(M.getContext()), 2, "if_rel");
  PN->addIncoming(ThenVal, ThenBB);
  PN->addIncoming(ElseVal, ElseBB);
  return PN;
}
