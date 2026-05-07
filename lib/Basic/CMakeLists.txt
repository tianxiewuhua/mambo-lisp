llvm_add_library(mamboBasic STATIC Version.cpp Parser.cpp AST.cpp
                 TokenKinds.cpp Diagnostic.cpp)

target_link_libraries(mamboBasic INTERFACE $LLVM_COMMON_LIBS)
