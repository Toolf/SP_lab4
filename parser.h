#ifndef SP_PARSER
#define SP_PARSER

#include <string>
#include "lexer.h"
#include "chainMap.h"

struct ParserException : public std::exception
{
    std::string msg;

    ParserException(const char *msg)
    {
        this->msg = msg;
    }
};

class Node
{
public:
    virtual ~Node() = default;
};

class Program;
class Function;

class Statement;
class Exp;
class Return;
class Declare;
class Compaund;
class Conditional;

class Expression;
class Assign;
class BinOp;
class UnaryOp;
class Var;
class Constant;
class CondExp;
class FunCall;

class Function : public Node
{
public:
    std::string name;
    std::vector<std::string> types;
    std::vector<std::string> parameters;
    std::vector<Statement *> statements;
    bool has_body;

    Function() {}

    Function(std::string name, std::vector<std::string> types, std::vector<std::string> parameters, std::vector<Statement *> statements)
    {
        this->name = name;
        this->types = types;
        this->parameters = parameters;
        this->statements = statements;

        this->has_body = true;
    }

    Function(std::string name, std::vector<std::string> types, std::vector<std::string> parameters)
    {
        this->name = name;
        this->types = types;
        this->parameters = parameters;
        this->statements = std::vector<Statement *>();

        this->has_body = false;
    }

    void add_body(std::vector<Statement *> statements)
    {
        this->statements = statements;
        this->has_body = true;
    }
};

class Program : public Node
{
public:
    std::vector<Function *> functions;
    Program(std::vector<Function *> functions)
    {
        this->functions = functions;
    }
};

class Statement : public Node
{
};

class Exp : public Statement
{
public:
    Expression *exp;
    Exp(Expression *exp)
    {
        this->exp = exp;
    }
};

class Return : public Statement
{
public:
    Expression *exp;
    Return(Expression *exp)
    {
        this->exp = exp;
    }
};

class Declare : public Statement
{
public:
    std::string type;
    std::string id;
    Expression *exp;
    Declare(std::string type, std::string id, Expression *exp)
    {
        this->type = type;
        this->id = id;
        this->exp = exp;
    }
};

class Compaund : public Statement
{
public:
    std::vector<Statement *> statements;

    Compaund(std::vector<Statement *> statements)
    {
        this->statements = statements;
    }
};

class Conditional : public Statement
{
public:
    Expression *exp;
    Statement *body;
    Statement *else_block;

    Conditional(Expression *exp, Statement *body, Statement *else_block)
    {
        this->exp = exp;
        this->body = body;
        this->else_block = else_block;
    }
};

class Expression : public Node
{
};

class Assign : public Expression
{
public:
    std::string id;
    Expression *exp;
    Assign(std::string id, Expression *exp)
    {
        this->id = id;
        this->exp = exp;
    }
};

class BinOp : public Expression
{
public:
    std::string op;
    Expression *left;
    Expression *right;
    BinOp(std::string op, Expression *left, Expression *right)
    {
        this->op = op;
        this->left = left;
        this->right = right;
    }
};

class UnaryOp : public Expression
{
public:
    std::string op;
    Expression *exp;
    UnaryOp(std::string op, Expression *exp)
    {
        this->op = op;
        this->exp = exp;
    }
};

class Var : public Expression
{
public:
    std::string name;
    Var(std::string name)
    {
        this->name = name;
    }
};

class Constant : public Expression
{
public:
    std::string type;
    std::string value;
    Constant(std::string type, std::string value)
    {
        this->type = type;
        this->value = value;
    }
};

class CondExp : public Expression
{
public:
    Expression *condition;
    Expression *if_exp;
    Expression *else_exp;
    CondExp(Expression *condition, Expression *if_exp, Expression *else_exp)
    {
        this->condition = condition;
        this->if_exp = if_exp;
        this->else_exp = else_exp;
    }
};

class CallFunction : public Expression
{
public:
    std::string name;
    std::vector<Expression *> arguments;
    CallFunction(std::string name, std::vector<Expression *> arguments)
    {
        this->name = name;
        this->arguments = arguments;
    }
};

class NullExpression : public Expression
{
};

class CParser
{
public:
    Program parse(CLexer *);

private:
    ChainMap<std::string, int> chain_var_map = ChainMap<std::string, int>();
    std::vector<Function *> functions;

    bool add_function(Function *function);
    bool function_exist(std::string name, int parameters_count);

    //-------------------------------

    Program *parse_program(CLexer *);

    Function *parse_function(CLexer *);

    Statement *parse_statement(CLexer *);

    Expression *parse_expression(CLexer *);

    Expression *parse_conditional_exp(CLexer *);

    Expression *parse_logical_or(CLexer *);

    Expression *parse_logical_and(CLexer *);

    Expression *parse_bit_or(CLexer *);

    Expression *parse_xor(CLexer *);

    Expression *parse_bit_and(CLexer *);

    Expression *parse_equals(CLexer *);

    Expression *parse_not_equals(CLexer *);

    Expression *parse_shift(CLexer *);

    Expression *parse_add(CLexer *);

    Expression *parse_term(CLexer *);

    Expression *parse_factor(CLexer *);

    Expression *parse_postfix(CLexer *);
};

bool add_function(Function *function, std::vector<Function> functions);

#endif // SP_PARSER