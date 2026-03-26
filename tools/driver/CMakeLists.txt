set(LLVM_LINK_COMPONENTS
  Support
)

add_llvm_executable(lispc
  Driver.cpp
)

target_link_libraries(lispc
  PRIVATE
  mamboBasic
)
