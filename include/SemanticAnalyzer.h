#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "AST.h"
#include "SymbolTable.h"
#include <vector>
#include <string>

class SemanticAnalyzer {
public:
    SemanticAnalyzer() = default;
    
    bool analyze(const ProgramNode& program);
    const SymbolTable& getSymbolTable() const { return symbolTable; }
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    SymbolTable symbolTable;
    std::vector<std::string> errors;

    void visitNode(const ASTNode* node);
    void visitVarDecl(const VarDeclNode* node);
    void visitAssignment(const AssignmentNode* node);
    std::string inferType(const ExprNode* expr);
};

#endif // SEMANTIC_ANALYZER_H
