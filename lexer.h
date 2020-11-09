
#ifndef SP_LEXER
#define SP_LEXER

#include <string>
#include <vector>
#include <map>
#include <set>

enum TokenTag
{
    RETURN, // 0
    IF,     // 1
    ELSE,   // 2
    COMMA,  // 3

    LPAR,   // 4
    RPAR,   // 5
    LBRACE, // 6
    RBRACE, // 7

    QUESTION, // 8
    COLON,    // 9

    SEMIC, // 10
    EQUAL, // 11

    NOT,     // 12
    TILDE,   // 13
    PLUS,    // 14
    MINUS,   // 15
    STAR,    // 16
    SLASH,   // 17
    PERCENT, // 18

    AMPERSAND,            // 19
    VERTICAL_LINE,        // 20
    DOUBLE_AMPERSAND,     // 21
    DOUBLE_VERTICAL_LINE, // 22
    CARET,                // 23

    LT,           // 24
    LTE,          // 25
    GT,           // 26
    GTE,          // 27
    DOUBLE_EQUAL, // 28
    NOT_EQUAL,    // 29
    LEFT_SHIFT,   // 30
    RIGHT_SHIFT,  // 31

    DOUBLE_MINUS, // 32
    DOUBLE_PLUS,  // 33

    INT,   // 34
    FLOAT, // 35
    CHAR,  // 36
    // ----------------
    CONSTANT, // 37
    ID,       // 38

    //-----------------
    UNKNOWN, // 39

    LastTokenTag // 40
};

const std::set<TokenTag> TYPES = {
    INT, FLOAT, CHAR};

const std::set<char> whiteList = {
    ' ',
    '\n',
    '\t',
};

const std::map<std::string, TokenTag> tokenTypes{
    {"return", RETURN},
    {"if", IF},
    {"else", ELSE},
    {",", COMMA},

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

    {"--", DOUBLE_MINUS},
    {"++", DOUBLE_PLUS},

    {"int", INT},
    {"float", FLOAT},
    {"char", CHAR},
};

std::string getNameOfTokenTag(TokenTag);

struct Token
{
    TokenTag tag;
    std::string value;
    int row;
    int col;
    Token(){};
    Token(TokenTag tag, std::string value, int row, int col);
};

struct LexerException : public std::exception
{
    std::string msg;

    LexerException(const char *msg)
    {
        this->msg = msg;
    }
};

class CLexer
{
public:
    CLexer(std::string code);
    Token nextToken();
    Token peek();
    Token last_taken_token();

    void saveState();
    void loadState();

    std::string code;
    int index = 0;
    int row = 0;
    int col = 0;

private:
    Token last_taken;
    int save_index;
    int save_row;
    int save_col;
};

#endif // SP_LEXER