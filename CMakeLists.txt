cmake_minimum_required(VERSION 3.0)
project (mooncC)

include_directories(./include)

set(CMAKE_CXX_FLAGS,${CMAKE_CXX_FLAGS} -o -Wall -g -std=c++17)
set(CMAKE_BUILD_TYPE Debug)

set(src_list ./src/lexer.cc ./src/lexer_dfa.cc ./src/main.cc ./src/util.cc ./src/parser.cc ./src/ir_gen.cc ./src/block.cc ./src/ir.cc ./src/AST.cc ./src/target_gen.cc)
add_executable(mooncC ${src_list})