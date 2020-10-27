#include "lexer.h"
#include "parser.h"
#include "codeGenerator.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const std::string input_file = "input.txt";

int main(int argc, char* argv[] ){
    std::ifstream input(input_file);
    std::string code((std::istreambuf_iterator<char>(input)),
                      std::istreambuf_iterator<char>());
    
    CLexer lexer = CLexer(code);
    CParser parser = CParser();
    CodeGenerator codeGenerator;
    int a;
    try{
        std::cout << codeGenerator.generate(parser.parse(&lexer));
    } catch(LexerException& e){
        std::cout << e.msg << std::endl; 
    } catch (ParserException& e){
        std::cout << e.msg << std::endl;
    } catch (CodeGeneratorException& e){
        std::cout << e.msg << std::endl;
    }

    return 0;
}