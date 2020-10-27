# Lexer

## Token types:
    RETURN:                 "return"
    IF:                     "if"
    ELSE:                   "else"

    LPAR:                   "("
    RPAR:                   ")"
    LBRACE:                 "{"
    RBRACE:                 "}"

    QUESTION:               "?"
    COLON:                  ":"
    
    SEMIC:                  ";"
    EQUAL:                  "="
    
    NOT:                    "!"
    TILDE:                  "~"
    PLUST:                  "+"
    MINUS:                  "-"
    STAR:                   "*"
    SLASH:                  "/"
    PERCENT:                "%"

    AMPERSAND:              "&"
    VERTICAL_LINE:          "|"
    DOUBLE_AMPERSAND:       "&&"
    DOUBLE_VERTICAL_LINE:   "||"
    CARET:                  "^"

    LT:                     "<"
    LTE:                    "<="
    GT:                     ">"
    GTE:                    ">="
    DOUBLE_EQUAL:           "=="
    NOT_EQUAL:              "!="
    LEFT_SHIFT:             "<<"
    RIGHT_SHIFT:            ">>"

    INT:                    "int"
    FLOAT:                  "float"
    CHAR:                   "char"
    TYPES:  [INT, FLOAT, CHAR]
    
    CONSTANT:               "constant"
    ID:                     "id"
    
    /-----------------
    UNKNOWN

    WHITELIST:  [' ', '\n', '\t']


# Parser

## AST
    program = Program(function_declaration)
    function_declaration = Function(string name, array<statement> statements)
    statement = Exp(exp) 
                | Return(exp)
                | Declare(string type, string name[, exp])
                | Compaund(array<statement> statements)
                | Conditional(exp, array<statement> statements[, array<statement> else])
    exp = Assign(string id, exp expression) 
        | BinOp(string op, exp left, exp right)
        | UnaryOp(string op, exp expression)
        | Var(string id)
        | Constant(string value, string type)
        | CondExp(exp condition, exp if_exp, exp else_exp)

## Grammer
    <program> ::= <function>
    <function> ::= "int" id "(" ")" "{" { <statement> } "}"
    <statement> ::= "return" <exp> ";" 
                    | <exp> ";"
                    | <type> <id> [ = <exp> ] ";"
                    | "{" { <statement> } "}"
                    | "if" "(" <exp> ")" <statement> [ "else" <statement>]
    <exp> ::= <id> "=" <exp> 
            | <conditional-exp>
    <conditional-exp> ::= <log_or> ["?" <exp> ":" <conditional-exp>]
    <log_or> ::= <log_and> { "||" <log_and> } 
    <log_and> ::= <bit_or> { "&&" <bit_or> }
    <bit_or> ::= <xor> { "|" <xor> }
    <xor> ::= <bit_and> { "^" <bit_and> }
    <bit_and> ::= <equals> { "&" <equals> }
    <equals> ::= <not_equals> { ("==" | "!=") <not_equals> }
    <not_equas> ::= <shift> { ("<" | ">" | "<=" | ">=") <shift> }
    <shift> ::= <add> { ("<<" | ">>") <add> }
    <add> ::= <term> { ("+" | "-") <term> }
    <term> ::= <factor> { ("*" | "/" | "%") <factor> }
    <factor> ::=  "(" <exp> ")" 
                | <unary_op> <factor> 
                | id 
                | int 
                | float 
                | char

    <unary_op> ::= "!" | "~" | "-"
    <type> ::= int | float | char