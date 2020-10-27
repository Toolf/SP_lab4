cmake_minimum_required(VERSION 3.16.3)

# set the project name
project(lab3 VERSION 1.0)

# add the executable
add_executable(lab3 main.cpp)

add_library(ChainMap STATIC chainMap.cpp)
target_link_libraries(lab3 ChainMap)
add_library(Lexer STATIC lexer.cpp)
target_link_libraries(lab3 Lexer)
add_library(Parser STATIC parser.cpp)
target_link_libraries(lab3 Parser)
add_library(CodeGenerator STATIC codeGenerator.cpp)
target_link_libraries(lab3 CodeGenerator)
