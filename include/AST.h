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

// Literal Node (e.g., 100 or "compiler")
class LiteralNode : public ExprNode {
public:
    std::string value;
    std::string literalType; // "int" or "string"

    LiteralNode(std::string val, std::string type) 
        : value(std::move(val)), literalType(std::move(type)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "LiteralNode (" << literalType << "): " << value << "\n";
    }
};

// Identifier Node (e.g., variable names like score, name)
class IdentifierNode : public ExprNode {
public:
    std::string name;

    explicit IdentifierNode(std::string name) : name(std::move(name)) {}

    void print(int indent = 0) const override {
        std::string padding(indent * 2, ' ');
        std::cout << padding << "IdentifierNode: " << name << "\n";
    }
};

// Variable Declaration Node (e.g., int score = 100;)
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

// Assignment Node (e.g., score = 200;)
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
