#include "lexer.h"
#include "parser.h"
#include "codeGenerator.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <clocale>

const std::string input_file = "5-23-C++-IO-81-Smirnov.txt";
const std::string output_file = "5-23-C++-IO-81-Smirnov.asm";

int main(int argc, char *argv[])
{
    std::ifstream input(input_file);
    std::ofstream output(output_file);

    std::string code((std::istreambuf_iterator<char>(input)),
                     std::istreambuf_iterator<char>());

    input.close();

    CLexer lexer = CLexer(code);
    CParser parser = CParser();
    CodeGenerator codeGenerator;
    int a;
    try
    {
        output << codeGenerator.generate(parser.parse(&lexer));
    }
    catch (LexerException &e)
    {
        output << e.msg << std::endl;
        std::cout << e.msg << std::endl;
    }
    catch (ParserException &e)
    {
        output << e.msg << std::endl;
        std::cout << e.msg << std::endl;
    }
    catch (CodeGeneratorException &e)
    {
        output << e.msg << std::endl;
        std::cout << e.msg << std::endl;
    }

    output.close();
    return 0;
}