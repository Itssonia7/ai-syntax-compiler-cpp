#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<ProgramNode> parseProgram();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token peek() const;
    Token advance();
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& errorMessage);

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<VarDeclNode> parseVarDecl();
    std::unique_ptr<AssignmentNode> parseAssignment();
    std::unique_ptr<IfStmtNode> parseIfStmt();
    std::unique_ptr<WhileStmtNode> parseWhileStmt();
    std::unique_ptr<BlockStmtNode> parseBlock();

    // Expression Parsing Hierarchy (Precedence Levels)
    std::unique_ptr<ExprNode> parseExpression();
    std::unique_ptr<ExprNode> parseEquality();
    std::unique_ptr<ExprNode> parseComparison();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();
    std::unique_ptr<ExprNode> parsePrimary();
};

#endif // PARSER_H
