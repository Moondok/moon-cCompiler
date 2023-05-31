cmake_minimum_required(VERSION 3.0)
project (moonCC)

include_directories(./include)

set(CMAKE_CXX_FLAGS,${CMAKE_CXX_FLAGS} -o -Wall -g -std=c++17)
set(CMAKE_BUILD_TYPE Debug)

set(src_list ./src/lexer.cc ./src/lexer_dfa.cc ./src/main.cc ./src/util.cc ./src/parser.cc ./src/ir_gen.cc ./src/block.cc ./src/ir.cc ./src/AST.cc ./src/target_gen.cc)

add_executable(moonCC ${src_list})
# add llvm setting
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})

target_include_directories(moonCC
    SYSTEM PUBLIC
        ${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(llvm_libs CORE)

target_link_libraries(moonCC
    PRIVATE
        ${llvm_libs}
)

