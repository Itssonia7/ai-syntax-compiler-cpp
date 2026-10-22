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
            // Error recovery: synchronize on semicolon or brace
            while (peek().type != TokenType::END_OF_FILE && 
                   peek().type != TokenType::SEMICOLON && 
                   peek().type != TokenType::RBRACE) {
                advance();
            }
            if (peek().type == TokenType::SEMICOLON || peek().type == TokenType::RBRACE) {
                advance();
            }
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
    } else if (current.type == TokenType::KEYWORD_IF) {
        return parseIfStmt();
    } else if (current.type == TokenType::KEYWORD_WHILE) {
        return parseWhileStmt();
    } else if (current.type == TokenType::LBRACE) {
        return parseBlock();
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

std::unique_ptr<IfStmtNode> Parser::parseIfStmt() {
    advance(); // Consume 'if'
    expect(TokenType::LPAREN, "Expected '(' after 'if'");
    auto cond = parseExpression();
    expect(TokenType::RPAREN, "Expected ')' after if condition");
    
    auto thenBranch = parseStatement();
    std::unique_ptr<ASTNode> elseBranch = nullptr;

    if (match(TokenType::KEYWORD_ELSE)) {
        elseBranch = parseStatement();
    }

    return std::make_unique<IfStmtNode>(std::move(cond), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<WhileStmtNode> Parser::parseWhileStmt() {
    advance(); // Consume 'while'
    expect(TokenType::LPAREN, "Expected '(' after 'while'");
    auto cond = parseExpression();
    expect(TokenType::RPAREN, "Expected ')' after while condition");
    
    auto body = parseStatement();
    return std::make_unique<WhileStmtNode>(std::move(cond), std::move(body));
}

std::unique_ptr<BlockStmtNode> Parser::parseBlock() {
    expect(TokenType::LBRACE, "Expected '{'");
    auto block = std::make_unique<BlockStmtNode>();

    while (peek().type != TokenType::RBRACE && peek().type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
    }

    expect(TokenType::RBRACE, "Expected '}' at end of block");
    return block;
}

// Expression Precedence Hierarchy

std::unique_ptr<ExprNode> Parser::parseExpression() {
    return parseEquality();
}

std::unique_ptr<ExprNode> Parser::parseEquality() {
    auto left = parseComparison();

    while (peek().type == TokenType::EQUAL_EQUAL || peek().type == TokenType::NOT_EQUAL) {
        Token op = advance();
        auto right = parseComparison();
        left = std::make_unique<BinaryExprNode>(std::move(left), op.value, std::move(right));
    }

    return left;
}

std::unique_ptr<ExprNode> Parser::parseComparison() {
    auto left = parseTerm();

    while (peek().type == TokenType::LESS || peek().type == TokenType::LESS_EQUAL ||
           peek().type == TokenType::GREATER || peek().type == TokenType::GREATER_EQUAL) {
        Token op = advance();
        auto right = parseTerm();
        left = std::make_unique<BinaryExprNode>(std::move(left), op.value, std::move(right));
    }

    return left;
}

std::unique_ptr<ExprNode> Parser::parseTerm() {
    auto left = parseFactor();

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token op = advance();
        auto right = parseFactor();
        left = std::make_unique<BinaryExprNode>(std::move(left), op.value, std::move(right));
    }

    return left;
}

std::unique_ptr<ExprNode> Parser::parseFactor() {
    auto left = parsePrimary();

    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        Token op = advance();
        auto right = parsePrimary();
        left = std::make_unique<BinaryExprNode>(std::move(left), op.value, std::move(right));
    }

    return left;
}

std::unique_ptr<ExprNode> Parser::parsePrimary() {
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
    } else if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    throw std::runtime_error("Expected literal, identifier, or '(' expression");
}
