#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    KEYWORD_INT,
    KEYWORD_STRING,
    KEYWORD_BOOLEAN,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,

    // Literals & Identifiers
    IDENTIFIER,
    INT_LITERAL,
    STRING_LITERAL,

    // Operators
    ASSIGN,         // =
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    GREATER,        // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=

    // Delimiters
    SEMICOLON,      // ;
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }

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
