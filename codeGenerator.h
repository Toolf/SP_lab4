#ifndef SP_CODEGENERATOR
#define SP_CODEGENERATOR

#include "parser.h"
#include "chainMap.h"

struct CodeGeneratorException : public std::exception
{
    std::string msg;

    CodeGeneratorException(const char *msg)
    {
        this->msg = msg;
    }
};

class CodeGenerator
{

public:
    ChainMap<std::string, int> chain_var_map;
    int offset;

    CodeGenerator();
    std::string generate(Program program);

    std::string generate(Function *function);

    std::string generate(Statement *statement);

    std::string generate(Exp exp);

    std::string generate(Return ret);

    std::string generate(Declare declare);

    std::string generate(Compaund compaund);

    std::string generate(Conditional conditional);

    std::string generate(Expression *expression);

    std::string generate(Assign assign);

    std::string generate(BinOp binop);

    std::string generate(UnaryOp unaryop);

    std::string generate(Var var);

    std::string generate(Constant constant);

    std::string generate(CondExp cond_exp);

    std::string generate(CallFunction callFunction);
};

#endif // SP_CODEGENERATOR