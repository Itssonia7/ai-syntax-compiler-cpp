#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

// Expression Base Node
class ExprNode : public ASTNode {};

// Literal Node
class LiteralNode : public ExprNode {
public:
    std::string value;
    std::string literalType;

    LiteralNode(std::string val, std::string type) 
        : value(std::move(val)), literalType(std::move(type)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "LiteralNode (" << literalType << "): " << value << "\n";
    }
};

// Identifier Node
class IdentifierNode : public ExprNode {
public:
    std::string name;

    explicit IdentifierNode(std::string name) : name(std::move(name)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "IdentifierNode: " << name << "\n";
    }
};

// Binary Expression Node (Arithmetic & Comparisons)
class BinaryExprNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> left;
    std::string op;
    std::unique_ptr<ExprNode> right;

    BinaryExprNode(std::unique_ptr<ExprNode> l, std::string op, std::unique_ptr<ExprNode> r)
        : left(std::move(l)), op(std::move(op)), right(std::move(r)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "BinaryExprNode (" << op << "):\n";
        if (left) left->print(indent + 1);
        if (right) right->print(indent + 1);
    }
};

// Variable Declaration Node
class VarDeclNode : public ASTNode {
public:
    std::string type;
    std::string name;
    std::unique_ptr<ExprNode> initializer;

    VarDeclNode(std::string type, std::string name, std::unique_ptr<ExprNode> init)
        : type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "VarDeclNode (" << type << " " << name << "):\n";
        if (initializer) {
            initializer->print(indent + 1);
        }
    }
};

// Assignment Node
class AssignmentNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ExprNode> value;

    AssignmentNode(std::string name, std::unique_ptr<ExprNode> val)
        : name(std::move(name)), value(std::move(val)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "AssignmentNode (" << name << "):\n";
        if (value) {
            value->print(indent + 1);
        }
    }
};

// Block Statement Node { ... }
class BlockStmtNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "BlockStmtNode\n";
        for (const auto& stmt : statements) {
            stmt->print(indent + 1);
        }
    }
};

// If Statement Node
class IfStmtNode : public ASTNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch;

    IfStmtNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ASTNode> thenB, std::unique_ptr<ASTNode> elseB = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "IfStmtNode:\n";
        std::cout << padding << "  Condition:\n";
        if (condition) condition->print(indent + 2);
        std::cout << padding << "  Then:\n";
        if (thenBranch) thenBranch->print(indent + 2);
        if (elseBranch) {
            std::cout << padding << "  Else:\n";
            elseBranch->print(indent + 2);
        }
    }
};

// While Loop Node
class WhileStmtNode : public ASTNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ASTNode> body;

    WhileStmtNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ASTNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "WhileStmtNode:\n";
        std::cout << padding << "  Condition:\n";
        if (condition) condition->print(indent + 2);
        std::cout << padding << "  Body:\n";
        if (body) body->print(indent + 2);
    }
};

// Root Program Node
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "ProgramNode\n";
        for (const auto& stmt : statements) {
            stmt->print(indent + 1);
        }
    }
};

#endif // AST_H
