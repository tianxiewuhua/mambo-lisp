cmake_minimum_required(VERSION 4.3.0)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(mambo)

    find_package(LLVM REQUIRED HINTS "${LLVM_CMAKE_PATH}")
    list (APPEND CMAKE_MODULE_PATH ${LLVM_DIR})

    include(AddLLVM)
    include(HandleLLVMOptions)

    include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
    link_directories("${LLVM_LIBRARY_DIR}")

    set(MAMBO_BUILT_STANDALONE 1)
endif()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FALTAL_ERROR "In-source builds are not allowed")
endif()

set(MAMBO_VERSION_STRING "0.1")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/mambo/Basic/Version.inc.in ${CMAKE_CURRENT_BINARY_DIR}/include/mambo/Basic/Version.inc)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

add_subdirectory(lib)
add_subdirectory(tools)
