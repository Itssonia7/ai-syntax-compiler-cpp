#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), pos(0) {}

Token Parser::peek() const {
    if (pos >= tokens.size()) return Token{TokenType::END_OF_FILE, "", 0, 0};
    return tokens[pos];
}

Token Parser::advance() {
    if (pos < tokens.size()) return tokens[pos++];
    return Token{TokenType::END_OF_FILE, "", 0, 0};
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type, const std::string& errorMessage) {
    if (peek().type == type) {
        return advance();
    }
    std::cerr << "[Syntax Error] Line " << peek().line << ", Col " << peek().column 
              << ": " << errorMessage << " (Found '" << peek().value << "')\n";
    throw std::runtime_error(errorMessage);
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    while (peek().type != TokenType::END_OF_FILE) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
        } catch (const std::exception& e) {
            // Error recovery: advance to semicolon to attempt parsing next statement
            while (peek().type != TokenType::END_OF_FILE && peek().type != TokenType::SEMICOLON) {
                advance();
            }
            if (peek().type == TokenType::SEMICOLON) advance();
        }
    }
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    Token current = peek();
    if (current.type == TokenType::KEYWORD_INT || 
        current.type == TokenType::KEYWORD_STRING || 
        current.type == TokenType::KEYWORD_BOOLEAN) {
        return parseVarDecl();
    } else if (current.type == TokenType::IDENTIFIER) {
        return parseAssignment();
    }
    
    std::cerr << "[Syntax Error] Line " << current.line << ", Col " << current.column 
              << ": Unexpected statement starting with '" << current.value << "'\n";
    advance();
    return nullptr;
}

std::unique_ptr<VarDeclNode> Parser::parseVarDecl() {
    Token typeTok = advance();
    Token idTok = expect(TokenType::IDENTIFIER, "Expected variable name after type keyword");

    std::unique_ptr<ExprNode> initExpr = nullptr;
    if (match(TokenType::ASSIGN)) {
        initExpr = parseExpression();
    }

    expect(TokenType::SEMICOLON, "Expected ';' at end of declaration");
    return std::make_unique<VarDeclNode>(typeTok.value, idTok.value, std::move(initExpr));
}

std::unique_ptr<AssignmentNode> Parser::parseAssignment() {
    Token idTok = expect(TokenType::IDENTIFIER, "Expected variable name");
    expect(TokenType::ASSIGN, "Expected '=' in assignment");
    auto expr = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' at end of assignment");
    return std::make_unique<AssignmentNode>(idTok.value, std::move(expr));
}

std::unique_ptr<ExprNode> Parser::parseExpression() {
    Token tok = peek();
    if (tok.type == TokenType::INT_LITERAL) {
        advance();
        return std::make_unique<LiteralNode>(tok.value, "int");
    } else if (tok.type == TokenType::STRING_LITERAL) {
        advance();
        return std::make_unique<LiteralNode>(tok.value, "string");
    } else if (tok.type == TokenType::IDENTIFIER) {
        advance();
        return std::make_unique<IdentifierNode>(tok.value);
    }
    
    throw std::runtime_error("Expected literal or identifier expression");
}
