llvm_add_library(mamboBasic STATIC Version.cpp Lex.cpp Parser.cpp AST.cpp)

target_link_libraries(mamboBasic INTERFACE $LLVM_COMMON_LIBS)
