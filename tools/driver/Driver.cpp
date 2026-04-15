#include "mambo/Basic/Ast.h"
#include "mambo/Basic/Lex.h"
#include "mambo/Basic/Parser.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <string>
#include <vector>

static llvm::cl::opt<std::string> InputFile(llvm::cl::Positional,
                                            llvm::cl::desc("<InputFile>"));

int main(int argc_, const char **argv_) {
  llvm::InitLLVM X(argc_, argv_);
  llvm::cl::ParseCommandLineOptions(argc_, argv_);

  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> FileOrErr =
      llvm::MemoryBuffer::getFile(InputFile);

  if (std::error_code BufferError = FileOrErr.getError()) {
    llvm::WithColor::error(llvm::errs(), argv_[0])
        << "Reading File Failed " << InputFile << ": " << BufferError.message()
        << "\n";
    exit(1);
  }

  Lexer lexer(FileOrErr.get()->getBuffer());
  SExprParser parser(lexer);
  std::unique_ptr<TransitionUnit> TransitionUnit = parser.parseTransitionUnit();

  llvm::LLVMContext C;
  llvm::Module M(InputFile.getValue(), C);
  llvm::IRBuilder<> B(C);

  std::vector<llvm::Value *> Vs = TransitionUnit->codegen(M, B);

  return 0;
}
