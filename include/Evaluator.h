#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "AST.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <variant>

// Support runtime integer and string values
using Value = std::variant<int, std::string>;

class Evaluator {
public:
    Evaluator() = default;

    void execute(const ProgramNode& program);
    void printRuntimeState() const;

private:
    std::unordered_map<std::string, Value> environment;

    void executeNode(const ASTNode* node);
    Value evaluateExpr(const ExprNode* expr);
    bool isTruthy(const Value& val) const;
    void printValue(const Value& val) const;
};

#endif // EVALUATOR_H
