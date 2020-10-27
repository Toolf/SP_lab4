
#ifndef SP_LEXER
#define SP_LEXER

#include <string>
#include <vector>
#include <map>
#include <set>

enum TokenTag{
    RETURN,                 // 0
    IF,                     // 1
    ELSE,                   // 2

    LPAR,                   // 3
    RPAR,                   // 4
    LBRACE,                 // 5
    RBRACE,                 // 6

    QUESTION,               // 7
    COLON,                  // 8
    
    SEMIC,                  // 9
    EQUAL,                  // 10
    
    NOT,                    // 11
    TILDE,                  // 12
    PLUS,                   // 13
    MINUS,                  // 14
    STAR,                   // 15
    SLASH,                  // 16
    PERCENT,                // 17

    AMPERSAND,              // 18
    VERTICAL_LINE,          // 19
    DOUBLE_AMPERSAND,       // 20
    DOUBLE_VERTICAL_LINE,   // 21
    CARET,                  // 22

    LT,                     // 23
    LTE,                    // 24
    GT,                     // 25
    GTE,                    // 26
    DOUBLE_EQUAL,           // 27
    NOT_EQUAL,              // 28
    LEFT_SHIFT,             // 29
    RIGHT_SHIFT,            // 30

    INT,                    // 31
    FLOAT,                  // 32
    CHAR,                   // 33
    // ----------------
    CONSTANT,               // 34
    ID,                     // 35

    //-----------------
    UNKNOWN,                // 36

    LastTokenTag            // 37
};

const std::set<TokenTag> TYPES = {
    INT, FLOAT, CHAR
};

const std::set<char> whiteList = {
    ' ', '\n', '\t',
};

const std::map<std::string, TokenTag> tokenTypes{
    {"return", RETURN},
    {"if", IF},
    {"else", ELSE},

    {"(", LPAR},
    {")", RPAR},
    {"{", LBRACE},
    {"}", RBRACE},
    
    {"?", QUESTION},
    {":", COLON},

    {";", SEMIC},
    {"=", EQUAL},

    {"!", NOT},
    {"~", TILDE},
    {"+", PLUS},
    {"-", MINUS},
    {"*", STAR},
    {"/", SLASH},
    {"%", PERCENT},

    {"&", AMPERSAND},
    {"|", VERTICAL_LINE},
    {"&&", DOUBLE_AMPERSAND},
    {"||", DOUBLE_VERTICAL_LINE},
    {"^", CARET},

    {"<", LT},
    {"<=", LTE},
    {">", GT},
    {">=", GTE},
    {"==", DOUBLE_EQUAL},
    {"!=", NOT_EQUAL},
    {"<<", LEFT_SHIFT},
    {">>", RIGHT_SHIFT},

    {"int", INT},
    {"float", FLOAT},
    {"char", CHAR},

    {"constant", CONSTANT},
    {"id", ID},
};

std::string getNameOfTokenTag(TokenTag);

struct Token{
    TokenTag tag;
    std::string value;
    int row;
    int col;
    Token(TokenTag tag, std::string value, int row, int col);
};

struct LexerException: public std::exception{
    std::string msg;

    LexerException(const char* msg){
        this->msg = msg;
    }
};

class CLexer{
    public:
        CLexer(std::string code);
        Token nextToken();
        Token peek();

        void saveState();
        void loadState();
        
        std::string code;
        int index=0;
        int row=0;
        int col=0;
    private:
        int save_index;
        int save_row;
        int save_col;
};

#endif // SP_LEXER