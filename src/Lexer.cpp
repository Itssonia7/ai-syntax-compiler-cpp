#include "Lexer.h"
#include <cctype>

Lexer::Lexer(std::string source) 
    : source(std::move(source)), pos(0), line(1), column(1) {}

char Lexer::peek() const {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    if (pos >= source.length()) return '\0';
    char current = source[pos++];
    if (current == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return current;
}

void Lexer::skipWhitespace() {
    while (pos < source.length() && (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n')) {
        advance();
    }
}

Token Lexer::readIdentifierOrKeyword() {
    int startCol = column;
    std::string value;
    
    while (pos < source.length() && (std::isalnum(peek()) || peek() == '_')) {
        value += advance();
    }

    TokenType type = TokenType::IDENTIFIER;
    if (value == "int") type = TokenType::KEYWORD_INT;
    else if (value == "string") type = TokenType::KEYWORD_STRING;
    else if (value == "boolean") type = TokenType::KEYWORD_BOOLEAN;

    return Token{type, value, line, startCol};
}

Token Lexer::readNumber() {
    int startCol = column;
    std::string value;

    while (pos < source.length() && std::isdigit(peek())) {
        value += advance();
    }

    return Token{TokenType::INT_LITERAL, value, line, startCol};
}

Token Lexer::readString() {
    int startCol = column;
    advance(); // Skip opening quote '"'
    std::string value;

    while (pos < source.length() && peek() != '"') {
        value += advance();
    }

    if (peek() == '"') {
        advance(); // Skip closing quote '"'
    }

    return Token{TokenType::STRING_LITERAL, value, line, startCol};
}

Token Lexer::nextToken() {
    skipWhitespace();

    if (pos >= source.length()) {
        return Token{TokenType::END_OF_FILE, "", line, column};
    }

    char c = peek();
    int startCol = column;

    if (std::isalpha(c) || c == '_') {
        return readIdentifierOrKeyword();
    }

    if (std::isdigit(c)) {
        return readNumber();
    }

    if (c == '"') {
        return readString();
    }

    if (c == '=') {
        advance();
        return Token{TokenType::ASSIGN, "=", line, startCol};
    }

    if (c == ';') {
        advance();
        return Token{TokenType::SEMICOLON, ";", line, startCol};
    }

    // Unknown token
    std::string unknownVal(1, advance());
    return Token{TokenType::UNKNOWN, unknownVal, line, startCol};
}

std::vector<Token> Lexer::tokenizeAll() {
    std::vector<Token> tokens;
    Token tok = nextToken();
    while (tok.type != TokenType::END_OF_FILE) {
        tokens.push_back(tok);
        tok = nextToken();
    }
    tokens.push_back(tok); // Append END_OF_FILE token
    return tokens;
}
