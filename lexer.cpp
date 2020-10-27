#include "lexer.h"
#include <iostream>

#define MIN(a,b) ((a) < (b) ? (a) : (b))


std::string getNameOfTokenTag(TokenTag t){
    auto it = tokenTypes.begin();
    while(it != tokenTypes.end()){
        if (it->second == t){
            return it->first;
        }
        it++;
    }
    return "";
}

Token::Token(TokenTag tag, std::string value, int row, int col){
    this->tag = tag;
    this->value = value;
    this->row = row;
    this->col = col;
}

CLexer::CLexer(std::string code){
    this->code = code;
    this->index = 0;
    this->row = 1;
    this->col = 1;
};


Token get_int_or_float(std::string code, CLexer* lexer){
    int start_index = lexer->index;
    int start_col = lexer->col;
    int was_dot = false;
    while(lexer->index < code.size() && (isdigit(code[lexer->index]) || code[lexer->index] == '.')){
        if (code[lexer->index] == '.'){
            if (was_dot){
                throw LexerException("Bad float");
            }
            was_dot = true;
        }
        lexer->index++;
        lexer->col++;
    }

    std::string value = code.substr(start_index, lexer->index-start_index);
    return Token(CONSTANT, value, lexer->row, start_col);
}

Token get_var_or_keyword(std::string code, CLexer* lexer){
    // Take name
    int start_index = lexer->index;
    int start_col = lexer->col;
    while(isalnum(code[lexer->index]) || code[lexer->index] == '_'){
        lexer->index++;
        lexer->col++;
    }
    std::string name = code.substr(start_index, lexer->index-start_index);
    std::map<std::string, TokenTag>::const_iterator it = tokenTypes.find(name);
    if (it == tokenTypes.end()){
        return Token(ID, name, lexer->row, start_col);
    } else {
        return Token(it->second, name, lexer->row, start_col);
    }
}

Token CLexer::nextToken(){
    // white list
    while (whiteList.find(code[index]) != whiteList.end())
    {
        if (code[index] == '\n'){
            this->row++;
            this->col = 1;
        } else {
            this->col++;
        }
        index++;
    }
    
    if (index >= code.size()){
        throw LexerException("EOF");
    }

    // int or float
    if (isdigit(code[index]) || code[index] == '.'){
        return get_int_or_float(code, this);
    }
    // var or keyword
    else if (isalpha(code[index]) || code[index] == '_')
    {   
        return get_var_or_keyword(code, this);
    }
    // other
    else
    {
        int char_count = MIN(this->index+3, this->code.size()) - this->index;
        std::map<std::string, TokenTag>::const_iterator it;
        while(char_count>0){
            std::string value = code.substr(this->index, char_count);
            it = tokenTypes.find(value);
            if (it != tokenTypes.end()){
                int col = this->col;
                this->col += char_count;
                this->index += char_count;
                return Token(it->second, value, this->row, col);
            }
            char_count--;
        }
        this->index++;
        this->col++;
        std::cout << this->code[this->index-1] << std::endl;
        return Token(UNKNOWN, this->code.substr(this->index-1, 1), this->row, this->col-1);
    }
    throw LexerException("EOF");
};

Token CLexer::peek(){
    int index = this->index;
    int row   = this->row;
    int col   = this->col;
    Token t = this->nextToken();
    this->index = index;
    this->row   = row;
    this->col   = col;
    return t;
};

void CLexer::saveState(){
    this->save_index = this->index;
    this->save_col = this->col;
    this->save_row = this->row;
};

void CLexer::loadState(){
    this->index = this->save_index;
    this->col = this->save_col;
    this->row = this->save_row;
}