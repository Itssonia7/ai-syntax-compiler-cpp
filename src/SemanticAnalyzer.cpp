#include "SemanticAnalyzer.h"

bool SemanticAnalyzer::analyze(const ProgramNode& program) {
    errors.clear();
    for (const auto& stmt : program.statements) {
        visitNode(stmt.get());
    }
    return errors.empty();
}

void SemanticAnalyzer::visitNode(const ASTNode* node) {
    if (!node) return;

    if (auto varDecl = dynamic_cast<const VarDeclNode*>(node)) {
        visitVarDecl(varDecl);
    } else if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        visitAssignment(assign);
    } else if (auto ifStmt = dynamic_cast<const IfStmtNode*>(node)) {
        visitIfStmt(ifStmt);
    } else if (auto whileStmt = dynamic_cast<const WhileStmtNode*>(node)) {
        visitWhileStmt(whileStmt);
    } else if (auto block = dynamic_cast<const BlockStmtNode*>(node)) {
        visitBlock(block);
    }
}

void SemanticAnalyzer::visitVarDecl(const VarDeclNode* node) {
    if (!symbolTable.insert(node->name, node->type, 1)) {
        errors.push_back("[Semantic Error] Redeclaration of variable '" + node->name + "'");
        return;
    }

    if (node->initializer) {
        std::string initType = inferType(node->initializer.get());
        if (!initType.empty() && initType != node->type) {
            errors.push_back("[Semantic Error] Type mismatch: Cannot assign '" + initType + 
                             "' to variable '" + node->name + "' of type '" + node->type + "'");
        }
    }
}

void SemanticAnalyzer::visitAssignment(const AssignmentNode* node) {
    Symbol* sym = symbolTable.lookup(node->name);
    if (!sym) {
        errors.push_back("[Semantic Error] Assignment to undeclared variable '" + node->name + "'");
        return;
    }

    if (node->value) {
        std::string valType = inferType(node->value.get());
        if (!valType.empty() && valType != sym->type) {
            errors.push_back("[Semantic Error] Type mismatch: Cannot assign '" + valType + 
                             "' to variable '" + node->name + "' of type '" + sym->type + "'");
        }
    }
}

void SemanticAnalyzer::visitIfStmt(const IfStmtNode* node) {
    if (node->condition) {
        std::string condType = inferType(node->condition.get());
        if (!condType.empty() && condType != "boolean" && condType != "int") {
            errors.push_back("[Semantic Error] 'if' condition must evaluate to boolean or int expression");
        }
    }
    visitNode(node->thenBranch.get());
    if (node->elseBranch) visitNode(node->elseBranch.get());
}

void SemanticAnalyzer::visitWhileStmt(const WhileStmtNode* node) {
    if (node->condition) {
        std::string condType = inferType(node->condition.get());
        if (!condType.empty() && condType != "boolean" && condType != "int") {
            errors.push_back("[Semantic Error] 'while' condition must evaluate to boolean or int expression");
        }
    }
    visitNode(node->body.get());
}

void SemanticAnalyzer::visitBlock(const BlockStmtNode* node) {
    for (const auto& stmt : node->statements) {
        visitNode(stmt.get());
    }
}

std::string SemanticAnalyzer::inferType(const ExprNode* expr) {
    if (!expr) return "";

    if (auto lit = dynamic_cast<const LiteralNode*>(expr)) {
        return lit->literalType;
    } 
    
    if (auto id = dynamic_cast<const IdentifierNode*>(expr)) {
        Symbol* sym = symbolTable.lookup(id->name);
        if (!sym) {
            errors.push_back("[Semantic Error] Reference to undeclared variable '" + id->name + "'");
            return "";
        }
        return sym->type;
    } 
    
    if (auto bin = dynamic_cast<const BinaryExprNode*>(expr)) {
        std::string leftType = inferType(bin->left.get());
        std::string rightType = inferType(bin->right.get());

        if (leftType.empty() || rightType.empty()) return "";

        // Relational operators produce boolean expressions
        if (bin->op == "==" || bin->op == "!=" || bin->op == "<" || 
            bin->op == ">"  || bin->op == "<=" || bin->op == ">=") {
            if (leftType != rightType) {
                errors.push_back("[Semantic Error] Type mismatch in comparison '" + bin->op + 
                                 "': '" + leftType + "' vs '" + rightType + "'");
            }
            return "boolean";
        }

        // Arithmetic operators require matching numeric types
        if (leftType != rightType) {
            errors.push_back("[Semantic Error] Type mismatch in arithmetic operation '" + bin->op + 
                             "': '" + leftType + "' vs '" + rightType + "'");
            return "";
        }

        return leftType;
    }

    return "";
}
