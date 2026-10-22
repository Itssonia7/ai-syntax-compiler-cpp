#include "SemanticAnalyzer.h"

bool SemanticAnalyzer::analyze(const ProgramNode& program) {
    errors.clear();
    for (const auto& stmt : program.statements) {
        visitNode(stmt.get());
    }
    return errors.empty();
}

void SemanticAnalyzer::visitNode(const ASTNode* node) {
    if (auto varDecl = dynamic_cast<const VarDeclNode*>(node)) {
        visitVarDecl(varDecl);
    } else if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        visitAssignment(assign);
    }
}

void SemanticAnalyzer::visitVarDecl(const VarDeclNode* node) {
    // Check for duplicate variable declaration
    if (!symbolTable.insert(node->name, node->type, 1)) {
        errors.push_back("[Semantic Error] Redeclaration of variable '" + node->name + "'");
        return;
    }

    // Type checking for initializer expression
    if (node->initializer) {
        std::string initType = inferType(node->initializer.get());
        if (!initType.empty() && initType != node->type) {
            errors.push_back("[Semantic Error] Type mismatch: Cannot assign '" + initType + 
                             "' to variable '" + node->name + "' of type '" + node->type + "'");
        }
    }
}

void SemanticAnalyzer::visitAssignment(const AssignmentNode* node) {
    // Verify variable exists in symbol table
    Symbol* sym = symbolTable.lookup(node->name);
    if (!sym) {
        errors.push_back("[Semantic Error] Assignment to undeclared variable '" + node->name + "'");
        return;
    }

    // Type checking for assigned value
    if (node->value) {
        std::string valType = inferType(node->value.get());
        if (!valType.empty() && valType != sym->type) {
            errors.push_back("[Semantic Error] Type mismatch: Cannot assign '" + valType + 
                             "' to variable '" + node->name + "' of type '" + sym->type + "'");
        }
    }
}

std::string SemanticAnalyzer::inferType(const ExprNode* expr) {
    if (auto lit = dynamic_cast<const LiteralNode*>(expr)) {
        return lit->literalType;
    } else if (auto id = dynamic_cast<const IdentifierNode*>(expr)) {
        Symbol* sym = symbolTable.lookup(id->name);
        if (!sym) {
            errors.push_back("[Semantic Error] Reference to undeclared variable '" + id->name + "'");
            return "";
        }
        return sym->type;
    }
    return "";
}
