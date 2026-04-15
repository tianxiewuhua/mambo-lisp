#include "mambo/Basic/Lex.h"
#include "llvm/ADT/StringRef.h"
#include <cctype>

static const char *KW_DEFUN = "defun";

static bool is_function_punct(char _c) {
  switch (_c) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '?':
  case '!':
    return true;
  default:
    return false;
  }
}

void Lexer::next(Token &Tok) {
  while (*BufferPtr && isspace(*BufferPtr)) {
    ++BufferPtr;
  }

  if (!*BufferPtr) {
    Tok.Kind = Token::eoi;
    return;
  }

  if (isalpha(*BufferPtr) || is_function_punct(*BufferPtr)) {
    const char *BufferEnd = BufferPtr + 1;

    while (isalnum(*BufferEnd) || is_function_punct(*BufferEnd)) {
      ++BufferEnd;
    }

    llvm::StringRef Name(BufferPtr, BufferEnd - BufferPtr);
    Tok.Kind = Name == KW_DEFUN ? Token::defun : Token::symbol;
    Tok.Text = Name;
    BufferPtr = BufferEnd;
    return;
  }

  if (isdigit(*BufferPtr) || *BufferPtr == '.') {
    const char *BufferEnd = BufferPtr + 1;
    while (isdigit(*BufferEnd) || *BufferEnd == '.') {
      ++BufferEnd; 
    }

    llvm::StringRef NumberStr(BufferPtr, BufferEnd - BufferPtr);
    Tok.Kind = Token::number;
    Tok.Text = NumberStr;

    BufferPtr = BufferEnd;
    return;
  } 

  if (*BufferPtr == '"') {
    const char *BufferEnd = BufferPtr + 1;
    while (*BufferEnd != '"') {
      ++BufferEnd;
    }

    llvm::StringRef Str(BufferPtr, BufferEnd - BufferPtr);
    Tok.Kind = Token::str;
    Tok.Text = Str;

    BufferPtr = BufferEnd;
    return;
  }  

  switch (*BufferPtr) {
    case '(':
      Tok.Kind = Token::lp;
      Tok.Text = "(";
      ++BufferPtr;
      break;
    case ')':
      Tok.Kind = Token::rp;
      Tok.Text = ")";
      ++BufferPtr;
      break;
    default:
      Tok.Kind = Token::unknown;
      Tok.Text = llvm::StringRef(BufferPtr, 1);
      ++BufferPtr;
      break;
  }
}

Token::TokenKind Token::getKind() {
  return Kind;
}

llvm::StringRef Token::getText() {
  return Text;
}
