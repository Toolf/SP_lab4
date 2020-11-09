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

    DOUBLE_MINUS:           "--"
    DOUBLE_PLUS:            "++"

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

    program = Program(array<function_declaration> functions)
    function_declaration = Function(string name, array<type> types, array<string> parameters, array<statement> statements OPTION)
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
        | CallFunction(string name, array<exp> arguments)

## Grammer

    <program> ::= { <function> }
    <function> ::= "int" id "(" [<type> <id> {"," <type> <id>}] ")" ( "{" { <statement> } "}" | ";")
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
                | id [ ( "(" [<exp> {"," <exp>}] ")" ) | ("++" | "--")]
                | int
                | float
                | char

    <unary_op> ::= "!" | "~" | "-"
    <type> ::= int | float | char
