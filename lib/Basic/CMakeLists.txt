llvm_add_library(mamboBasic STATIC Version.cpp)

target_link_libraries(mamboBasic INTERFACE $LLVM_COMMON_LIBS)
