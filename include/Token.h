#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    KEYWORD_INT,
    KEYWORD_STRING,
    KEYWORD_BOOLEAN,

    // Literals & Identifiers
    IDENTIFIER,
    INT_LITERAL,
    STRING_LITERAL,

    // Operators & Symbols
    ASSIGN,       // =
    SEMICOLON,    // ;

    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

#endif // TOKEN_H
