#include "parser.h"
#include "chainMap.h"
#include <iostream>
#include <sstream>
#include <vector>

void bad_code(std::string msg, Token t)
{
    std::stringstream exception_stringstream;
    exception_stringstream << msg
                           << " " << t.row << ":" << t.col << std::endl;

    throw ParserException(exception_stringstream.str().c_str());
}

bool add_function_to_vector(Function *function, std::vector<Function *> &functions)
{
    for (int i = 0; i < functions.size(); i++)
    {
        if (functions[i]->name == function->name)
        {
            if (functions[i]->parameters.size() != function->parameters.size())
            {
                return false;
            }
            if (functions[i]->has_body && function->has_body)
            {
                return false;
            }
            else if (functions[i]->has_body && !function->has_body)
            {
                return true;
            }
            else if (function->has_body)
            {
                functions[i] = function;
                return true;
            }
            else
            {
                return true;
            }
        }
    }
    functions.push_back(function);
    return true;
}

bool CParser::add_function(Function *function)
{
    add_function_to_vector(function, this->functions);
}

bool CParser::function_exist(std::string name, int parameters_count)
{
    for (int i = 0; i < functions.size(); i++)
    {
        if (functions[i]->name == name)
        {
            return functions[i]->parameters.size() == parameters_count;
        }
    }
    return false;
}

Program CParser::parse(CLexer *lexer)
{
    return *parse_program(lexer);
}

Program *CParser::parse_program(CLexer *lexer)
{
    // <program> ::= { <function> }
    std::vector<Function *> functions;
    try
    {
        while (1)
        {
            add_function_to_vector(parse_function(lexer), functions);
        }
    }
    catch (LexerException &e)
    {
        for (int i = 0; i < functions.size(); i++)
        {
            if (!functions[i]->has_body)
            {
                bad_code("Function " + functions[i]->name + " has no body", Token(LastTokenTag, "", lexer->row, lexer->col));
            }
        }
        return new Program(this->functions);
    }
}

Function *CParser::parse_function(CLexer *lexer)
{
    // <function> ::= "int" id "(" [<type> <id> {"," <type> <id>}] ")" ( "{" { <statement> } "}" | ";")
    std::string function_name;
    std::vector<Statement *> statements;
    std::vector<std::string> types;
    std::vector<std::string> parameters;

    std::string type;
    std::string name;

    Token start_token = lexer->peek();

    if (lexer->nextToken().tag == INT && lexer->peek().tag == ID)
    {
        function_name = lexer->nextToken().value;
        if (lexer->nextToken().tag == LPAR)
        {
            if (lexer->peek().tag != RPAR)
            {
                Token t = lexer->peek();
                if (t.tag == INT || t.tag == FLOAT || t.tag == CHAR)
                {
                    lexer->nextToken();
                    type = t.tag == INT ? "int" : t.tag == FLOAT ? "float" : "char";

                    if (lexer->peek().tag == ID)
                    {
                        name = lexer->nextToken().value;
                        types.push_back(type);
                        parameters.push_back(name);
                        this->chain_var_map.insert(name, 0);

                        while (lexer->peek().tag == COMMA)
                        {
                            lexer->nextToken();
                            Token t = lexer->peek();
                            if (t.tag == INT || t.tag == FLOAT || t.tag == CHAR)
                            {
                                lexer->nextToken();
                                type = t.tag == INT ? "int" : t.tag == FLOAT ? "float" : "char";

                                if (lexer->peek().tag == ID)
                                {
                                    name = lexer->nextToken().value;
                                }
                                else
                                {
                                    bad_code("Bad parameters", lexer->last_taken_token());
                                }
                            }
                            else
                            {
                                bad_code("Bad parameters", t);
                            }

                            types.push_back(type);
                            parameters.push_back(name);
                            this->chain_var_map.insert(name, 0);
                        }
                    }
                    else
                    {
                        bad_code("Bad parameters", lexer->peek());
                    }
                }
            }
            // if (!add_function(new Function(function_name, types, parameters)))
            // {
            //     bad_code("Double declaration", start_token);
            // }
            this->chain_var_map.new_map();
            if (lexer->nextToken().tag == RPAR)
            {
                if (lexer->peek().tag == LBRACE)
                {
                    lexer->nextToken();
                    while (lexer->peek().tag != RBRACE)
                    {
                        Statement *s = parse_statement(lexer);
                        statements.push_back(s);
                    }
                    lexer->nextToken();
                    Function *function = new Function(function_name, types, parameters, statements);
                    if (!add_function(function))
                    {
                        bad_code("Function redefinition error", start_token);
                    }
                    this->chain_var_map.remove_map();
                    return function;
                }
                else if (lexer->peek().tag == SEMIC)
                {
                    lexer->nextToken();
                    Function *function = new Function(function_name, types, parameters);
                    if (!add_function(function))
                    {
                        bad_code("Double declaration", start_token);
                    }
                    this->chain_var_map.remove_map();
                    return function;
                }
                else
                {
                    bad_code("Bad function", lexer->last_taken_token());
                }
            }
        }
    }
    bad_code("Bad function", lexer->last_taken_token());
}

Statement *CParser::parse_statement(CLexer *lexer)
{
    //  statement   ->  ("int"|"float"|"char") id ["=" expression] ";"
    //                  | "return" expression ";"
    //                  | "{" { statement } "}"
    //                  | "if" "(" expression ")" statement ["else" statement]
    //                  | expression ";"
    Token t = lexer->peek();
    if ((t.tag == INT || t.tag == FLOAT || t.tag == CHAR))
    {
        lexer->nextToken();
        std::string type = t.tag == INT ? "int" : t.tag == FLOAT ? "float" : "char";
        if (lexer->peek().tag == ID)
        {
            std::string name = lexer->nextToken().value;
            if (this->chain_var_map.exist_in_last_map(name))
            {
                bad_code("Var had been declare", lexer->last_taken_token());
            }
            if (lexer->peek().tag == EQUAL)
            {
                lexer->nextToken();
                Expression *exp = parse_expression(lexer);
                if (lexer->nextToken().tag == SEMIC)
                {
                    this->chain_var_map.insert(name, 0);
                    return (new Declare(type, name, exp));
                }
                else
                {
                    bad_code("Bad semicolumn", lexer->last_taken_token());
                }
            }
            if (lexer->nextToken().tag == SEMIC)
            {
                this->chain_var_map.insert(name, 0);
                return (new Declare(type, name, nullptr));
            }
            else
            {
                bad_code("Bad semicolumn", lexer->last_taken_token());
            }
        }
        bad_code("Bad declare", lexer->peek());
    }
    else if (t.tag == RETURN)
    {
        lexer->nextToken();
        Expression *exp = parse_expression(lexer);
        if (lexer->nextToken().tag == SEMIC)
        {
            return (new Return(exp));
        }
        bad_code("Bad return", lexer->last_taken_token());
    }
    else if (t.tag == LBRACE)
    {
        this->chain_var_map.new_map();

        lexer->nextToken();
        std::vector<Statement *> statements;
        while (lexer->peek().tag != RBRACE)
        {
            statements.push_back(parse_statement(lexer));
        }
        lexer->nextToken();

        this->chain_var_map.remove_map();
        return new Compaund(statements);
    }
    else if (t.tag == IF)
    {
        lexer->nextToken();
        if (lexer->nextToken().tag == LPAR)
        {
            Expression *exp = parse_expression(lexer);
            if (lexer->nextToken().tag == RPAR)
            {
                Statement *if_statement = parse_statement(lexer);
                Statement *else_statement = nullptr;
                if (lexer->peek().tag == ELSE)
                {
                    lexer->nextToken();
                    else_statement = parse_statement(lexer);
                }
                return new Conditional(exp, if_statement, else_statement);
            }
            else
            {
                bad_code("Bad brakets in if block", lexer->last_taken_token());
            }
        }
        else
        {
            bad_code("Bad if block", lexer->last_taken_token());
        }
    }
    else
    {
        Expression *exp = parse_expression(lexer);
        if (lexer->nextToken().tag == SEMIC)
        {
            return new Exp(exp);
        }
        bad_code("Bad exp(semicolumn at the end)", lexer->last_taken_token());
    }
}

Expression *CParser::parse_expression(CLexer *lexer)
{
    //  expression  ->  id "=" expression
    //                  | conditional-exp
    lexer->saveState();
    Token t = lexer->nextToken();
    if (t.tag == ID && lexer->nextToken().tag == EQUAL)
    {
        if (!this->chain_var_map.exist(t.value))
        {
            bad_code("Var hasn't been declare", t);
        }
        Expression *expression = parse_expression(lexer);
        this->chain_var_map.insert(t.value, 0);
        return new Assign(t.value, expression);
    }
    lexer->loadState();

    return parse_conditional_exp(lexer);
}

Expression *CParser::parse_conditional_exp(CLexer *lexer)
{
    Expression *condition = parse_logical_or(lexer);
    if (lexer->peek().tag == QUESTION)
    {
        lexer->nextToken();
        Expression *if_exp = parse_expression(lexer);
        if (lexer->nextToken().tag == COLON)
        {
            Expression *else_exp = parse_conditional_exp(lexer);
            return new CondExp(condition, if_exp, else_exp);
        }
        else
        {
            bad_code("Bad trinary operator", lexer->last_taken_token());
        }
    }
    else
    {
        return condition;
    }
}

Expression *CParser::parse_logical_or(CLexer *lexer)
{
    Expression *left = parse_logical_and(lexer);
    while (lexer->peek().tag == DOUBLE_VERTICAL_LINE)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_logical_and(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression *CParser::parse_logical_and(CLexer *lexer)
{
    Expression *left = parse_bit_or(lexer);
    while (lexer->peek().tag == DOUBLE_AMPERSAND)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_bit_or(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression *CParser::parse_bit_or(CLexer *lexer)
{
    Expression *left = parse_xor(lexer);
    while (lexer->peek().tag == VERTICAL_LINE)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_xor(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression *CParser::parse_xor(CLexer *lexer)
{
    Expression *left = parse_bit_and(lexer);
    while (lexer->peek().tag == CARET)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_bit_and(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression *CParser::parse_bit_and(CLexer *lexer)
{
    Expression *left = parse_equals(lexer);
    while (lexer->peek().tag == AMPERSAND)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_equals(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression *CParser::parse_equals(CLexer *lexer)
{
    Expression *left = parse_not_equals(lexer);
    TokenTag tag = lexer->peek().tag;
    if (tag == DOUBLE_EQUAL || tag == NOT_EQUAL)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_not_equals(lexer);
        left = new BinOp(op, left, right);

        tag = lexer->peek().tag;
    }
    return left;
}

Expression *CParser::parse_not_equals(CLexer *lexer)
{
    Expression *left = parse_shift(lexer);
    TokenTag tag = lexer->peek().tag;
    if (tag == LT || tag == GT || tag == LTE || tag == GTE)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_shift(lexer);
        left = new BinOp(op, left, right);

        tag = lexer->peek().tag;
    }
    return left;
}

Expression *CParser::parse_shift(CLexer *lexer)
{
    Expression *left = parse_add(lexer);
    TokenTag tag = lexer->peek().tag;
    while (tag == LEFT_SHIFT || tag == RIGHT_SHIFT)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_add(lexer);
        left = new BinOp(op, left, right);

        tag = lexer->peek().tag;
    }
    return left;
}

Expression *CParser::parse_add(CLexer *lexer)
{
    Expression *left = parse_term(lexer);
    TokenTag tag = lexer->peek().tag;
    while (tag == PLUS || tag == MINUS)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_term(lexer);
        left = new BinOp(op, left, right);

        tag = lexer->peek().tag;
    }
    return left;
}

Expression *CParser::parse_term(CLexer *lexer)
{
    Expression *left = parse_factor(lexer);
    TokenTag tag = lexer->peek().tag;
    while (tag == SLASH || tag == STAR || tag == PERCENT)
    {
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_factor(lexer);
        left = new BinOp(op, left, right);

        tag = lexer->peek().tag;
    }
    return left;
}

Expression *CParser::parse_factor(CLexer *lexer)
{
    Token t = lexer->nextToken();
    if (t.tag == LPAR)
    {
        Expression *exp = parse_expression(lexer);
        if (lexer->nextToken().tag == RPAR)
        {
            return exp;
        }
        else
        {
            bad_code("Bad brakets", lexer->last_taken_token());
        }
    }
    else if (t.tag == NOT || t.tag == TILDE || t.tag == MINUS)
    {
        std::string op = getNameOfTokenTag(t.tag);
        Expression *exp = parse_factor(lexer);
        return new UnaryOp(op, exp);
    }
    else if (t.tag == CONSTANT)
    {
        return new Constant("INT", t.value);
    }
    else if (t.tag == ID)
    {
        // id [ ( "(" [<exp> {"," <exp>}] ")" ) | ("++" | "--")]
        Token k = lexer->peek();
        if (k.tag == LPAR)
        {
            lexer->nextToken();
            std::vector<Expression *> argumets;

            if (lexer->peek().tag != RPAR)
            {
                argumets.push_back(parse_expression(lexer));
                while (lexer->peek().tag == COMMA)
                {
                    lexer->nextToken();
                    argumets.push_back(parse_expression(lexer));
                }
            }

            if (lexer->nextToken().tag == RPAR)
            {
                if (function_exist(t.value, argumets.size()))
                {
                    return new CallFunction(t.value, argumets);
                }
                else
                {

                    bad_code("Function " + t.value + " not found", t);
                }
            }
            else
            {
                bad_code("Bad call function", lexer->last_taken_token());
            }
        }
        else if (k.tag == DOUBLE_PLUS || k.tag == DOUBLE_MINUS)
        {
            if (!this->chain_var_map.exist(t.value))
            {
                bad_code("Var hasn't been declare", t);
            }
            lexer->nextToken();

            std::string op = k.tag == DOUBLE_PLUS ? "postfix inc" : "postfix dec";
            return new UnaryOp(op, new Var(t.value));
        }
        else
        {
            if (!this->chain_var_map.exist(t.value))
            {
                bad_code("Var hasn't been declare", t);
            }
            return new Var(t.value);
        }
    }
    // auto it = UNOP.find(t.tag);
    bad_code("Bad factor", t);
}