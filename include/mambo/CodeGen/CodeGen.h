#include "mambo/Basic/Ast.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace mambo {
namespace CodeGen {

class CodeGenFunctions {
private:
  llvm::IRBuilder<> &Builder;
  llvm::Module &M;

  llvm::Value *EmitSExpr(SExpr *S);

public:
  CodeGenFunctions(llvm::IRBuilder<> &Builder, llvm::Module &M)
      : Builder(Builder), M(M) {}

  std::vector<llvm::Value *> EmitTransitionUnit(TransitionUnit *T);

  llvm::Value *EmitVarExpr(VarExpr *V);
  llvm::Value *EmitVarDefExpr(VarDefExpr *VD);
  llvm::Value *EmitLetBindingsExpr(LetBindingsExpr *LB);

  llvm::Function *EmitFunctionProto(FunctionPrototype *FP);
  llvm::Value *EmitFunctionCallExpr(FunctionCallExpr *FC);
  llvm::Value *EmitFunctionDefExpr(FunctionDefineExpr *FD);

  llvm::Value *EmitNumerExpr(NumberExpr *N);
  llvm::Value *EmitStringExpr(StringExpr *Str);

  llvm::Value *EmitIfExpr(IfExpr *IF);
};

} // namespace CodeGen
} // namespace mambo
