#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string source);
    Token nextToken();
    std::vector<Token> tokenizeAll();

private:
    std::string source;
    size_t pos;
    int line;
    int column;

    char peek() const;
    char advance();
    void skipWhitespace();
    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString();
};

#endif // LEXER_H
