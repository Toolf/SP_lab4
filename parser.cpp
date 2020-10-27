#include "parser.h"
#include "chainMap.h"
#include <iostream>

Program CParser::parse(CLexer* lexer){
    Program p(parse_function(lexer));
    try{
        Token t = lexer->nextToken();
    }
    catch(LexerException& e){
        return p;
    }
    
    throw ParserException("Bad program");
}

Function CParser::parse_function(CLexer* lexer){
    std::string name;
    std::vector <Statement*> statements;

    if (lexer->nextToken().tag == INT && lexer->peek().tag == ID)
    {
        name = lexer->nextToken().value;
        if (lexer->nextToken().tag == LPAR 
            && lexer->nextToken().tag == RPAR 
            && lexer->nextToken().tag == LBRACE)
        {
            while (lexer->peek().tag != RBRACE)
            {
                Statement* s = parse_statement(lexer);
                statements.push_back(s);
            }
            lexer->nextToken();
            return Function(name, statements);   
        }
    }
    throw ParserException("Bad function");
}

Statement* CParser::parse_statement(CLexer* lexer){
//  statement   ->  ("int"|"float"|"char") id ["=" expression] ";"
//                  | "return" expression ";"
//                  | "{" { statement } "}"
//                  | "if" "(" expression ")" statement ["else" statement]
//                  | expression ";"
    Token t = lexer->peek();
    if ((t.tag == INT || t.tag == FLOAT || t.tag == CHAR)) {
        lexer->nextToken();
        std::string type = t.tag == INT ? "int" : t.tag == FLOAT ? "float" : "char";
        if (lexer->peek().tag == ID){
            std::string name = lexer->nextToken().value;
            if (this->chain_var_map.exist_in_last_map(name)){
                throw ParserException("Var have been declare");
            }
            if (lexer->peek().tag == EQUAL){
                lexer->nextToken();
                Expression* exp = parse_expression(lexer);
                if (lexer->nextToken().tag == SEMIC){
                    this->chain_var_map.insert(name, 0);
                    return (new Declare(type, name, exp));
                } else {
                    throw ParserException("Bad semicolumn");
                }
            }
            if (lexer->nextToken().tag == SEMIC){
                return (new Declare(type, name, nullptr));
            } else {
                throw ParserException("Bad semicolumn");
            }
        }
        throw ParserException("Bad declare");
    } else if (t.tag == RETURN) {
        lexer->nextToken();
        Expression* exp = parse_expression(lexer);
        if (lexer->nextToken().tag == SEMIC){
            return (new Return(exp));
        }
        throw ParserException("Bad return");
    } else if (t.tag == LBRACE){
        this->chain_var_map.new_map();

        lexer->nextToken();
        std::vector<Statement*> statements;
        while(lexer->peek().tag != RBRACE){
            statements.push_back(parse_statement(lexer));
        }
        lexer->nextToken();
        
        this->chain_var_map.remove_map();
        return new Compaund(statements);
    } else if (t.tag == IF){
        lexer->nextToken();
        if (lexer->nextToken().tag == LPAR){
            Expression* exp = parse_expression(lexer);
            if (lexer->nextToken().tag == RPAR){
                Statement* if_statement = parse_statement(lexer);
                Statement* else_statement = nullptr;
                if (lexer->peek().tag == ELSE){
                    lexer->nextToken();
                    else_statement = parse_statement(lexer);
                }
                return new Conditional(exp, if_statement, else_statement);
            } else {
                throw ParserException("Bad brakets in if block");
            }
        } else {
            throw ParserException("Bad if block");
        }
    } else {
        Expression* exp = parse_expression(lexer);
        if (lexer->nextToken().tag == SEMIC){
            return new Exp(exp);
        }
        throw ParserException("Bad exp");
    }
}

Expression* CParser::parse_expression(CLexer* lexer){
    //  expression  ->  id "=" expression
    //                  | conditional-exp
    lexer->saveState();
    Token t = lexer->nextToken();
    if (t.tag == ID && lexer->nextToken().tag == EQUAL){
        Expression* expression = parse_expression(lexer);
        this->chain_var_map.insert(t.value, 0);
        return new Assign(t.value, expression);
    }
    lexer->loadState();

    return parse_conditional_exp(lexer);
}

Expression* CParser::parse_conditional_exp(CLexer* lexer){
    Expression* condition = parse_logical_or(lexer);
    if (lexer->peek().tag == QUESTION){
        lexer->nextToken();
        Expression* if_exp = parse_expression(lexer);
        if (lexer->nextToken().tag == COLON){
            Expression* else_exp = parse_conditional_exp(lexer);
            return new CondExp(condition, if_exp, else_exp);
        } else {
            throw ParserException("Bad trinary operator");
        }
    } else {
        return condition;
    }
}

Expression* CParser::parse_logical_or(CLexer* lexer){
    Expression* left = parse_logical_and(lexer);
    while (lexer->peek().tag == DOUBLE_VERTICAL_LINE){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression* right = parse_logical_and(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression* CParser::parse_logical_and(CLexer* lexer){
    Expression* left = parse_bit_or(lexer);
    while (lexer->peek().tag == DOUBLE_AMPERSAND){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression* right = parse_bit_or(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression* CParser::parse_bit_or(CLexer* lexer){
    Expression* left = parse_xor(lexer);
    while (lexer->peek().tag == VERTICAL_LINE){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression* right = parse_xor(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression* CParser::parse_xor(CLexer* lexer){
    Expression* left = parse_bit_and(lexer);
    while (lexer->peek().tag == CARET){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression* right = parse_bit_and(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression* CParser::parse_bit_and(CLexer* lexer){
    Expression* left = parse_equals(lexer);
    while (lexer->peek().tag == AMPERSAND){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression* right = parse_equals(lexer);
        left = new BinOp(op, left, right);
    }
    return left;
}

Expression* CParser::parse_equals(CLexer* lexer){
    Expression* left = parse_not_equals(lexer);
    TokenTag tag = lexer->peek().tag;
    if (tag == DOUBLE_EQUAL || tag == NOT_EQUAL){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression* right = parse_not_equals(lexer);
        left = new BinOp(op, left, right);

        tag = lexer->peek().tag;
    }
    return left;
}

Expression* CParser::parse_not_equals(CLexer* lexer){
    Expression* left = parse_shift(lexer);
    TokenTag tag = lexer->peek().tag;
    if (tag == LT || tag == GT || tag == LTE || tag == GTE){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression* right = parse_shift(lexer);
        left = new BinOp(op, left, right);
        
        tag = lexer->peek().tag;
    }
    return left;
}

Expression* CParser::parse_shift(CLexer* lexer){
    Expression* left = parse_add(lexer);
    TokenTag tag = lexer->peek().tag;
    while (tag == LEFT_SHIFT || tag == RIGHT_SHIFT){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression* right = parse_add(lexer);
        left = new BinOp(op, left, right);
        
        tag = lexer->peek().tag;
    }
    return left;
}

Expression* CParser::parse_add(CLexer* lexer){
    Expression* left = parse_term(lexer);
    TokenTag tag = lexer->peek().tag;
    while(tag == PLUS || tag == MINUS){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_term(lexer);
        left = new BinOp(op, left, right);

        tag = lexer->peek().tag;
    }
    return left;
}

Expression* CParser::parse_term(CLexer* lexer){
    Expression* left = parse_factor(lexer);
    TokenTag tag = lexer->peek().tag;
    while(tag == SLASH || tag == STAR || tag == PERCENT){
        std::string op = getNameOfTokenTag(lexer->nextToken().tag);
        Expression *right = parse_factor(lexer);
        left = new BinOp(op, left, right);

        tag = lexer->peek().tag;
    }
    return left;        
}

Expression* CParser::parse_factor(CLexer* lexer){
    Token t = lexer->nextToken();
    if (t.tag == LPAR){
        Expression *exp = parse_expression(lexer);
        if (lexer->nextToken().tag == RPAR){
            return exp;
        } else {
            throw ParserException("Bad brakets");
        }
    } else if (t.tag == NOT || t.tag == TILDE || t.tag == MINUS) {
        std::string op = getNameOfTokenTag(t.tag);
        Expression *exp = parse_factor(lexer);
        return new UnaryOp(op, exp);
    } else if (t.tag == CONSTANT){
        return new Constant("INT", t.value);
    } else if (t.tag == ID){
        if (!this->chain_var_map.exist(t.value)){
            throw ParserException("Var hasn't been declare");
        }
        return new Var(t.value);
    }
    // auto it = UNOP.find(t.tag);
    // std::cout << t.col << " " << t.tag << std::endl;
    throw ParserException("Bad factor");
}