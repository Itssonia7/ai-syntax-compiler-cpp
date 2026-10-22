#include "Evaluator.h"
#include <stdexcept>
#include <iomanip>

void Evaluator::execute(const ProgramNode& program) {
    for (const auto& stmt : program.statements) {
        executeNode(stmt.get());
    }
}

void Evaluator::executeNode(const ASTNode* node) {
    if (!node) return;

    if (auto varDecl = dynamic_cast<const VarDeclNode*>(node)) {
        Value val = 0;
        if (varDecl->initializer) {
            val = evaluateExpr(varDecl->initializer.get());
        }
        environment[varDecl->name] = val;
    } 
    else if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        if (assign->value) {
            environment[assign->name] = evaluateExpr(assign->value.get());
        }
    } 
    else if (auto ifStmt = dynamic_cast<const IfStmtNode*>(node)) {
        Value condVal = evaluateExpr(ifStmt->condition.get());
        if (isTruthy(condVal)) {
            executeNode(ifStmt->thenBranch.get());
        } else if (ifStmt->elseBranch) {
            executeNode(ifStmt->elseBranch.get());
        }
    } 
    else if (auto whileStmt = dynamic_cast<const WhileStmtNode*>(node)) {
        while (isTruthy(evaluateExpr(whileStmt->condition.get()))) {
            executeNode(whileStmt->body.get());
        }
    } 
    else if (auto block = dynamic_cast<const BlockStmtNode*>(node)) {
        for (const auto& stmt : block->statements) {
            executeNode(stmt.get());
        }
    }
}

Value Evaluator::evaluateExpr(const ExprNode* expr) {
    if (!expr) return 0;

    if (auto lit = dynamic_cast<const LiteralNode*>(expr)) {
        if (lit->literalType == "int") {
            return std::stoi(lit->value);
        }
        return lit->value;
    }

    if (auto id = dynamic_cast<const IdentifierNode*>(expr)) {
        if (environment.find(id->name) != environment.end()) {
            return environment[id->name];
        }
        throw std::runtime_error("Runtime Error: Undefined variable '" + id->name + "'");
    }

    if (auto bin = dynamic_cast<const BinaryExprNode*>(expr)) {
        Value leftVal = evaluateExpr(bin->left.get());
        Value rightVal = evaluateExpr(bin->right.get());

        if (std::holds_alternative<int>(leftVal) && std::holds_alternative<int>(rightVal)) {
            int l = std::get<int>(leftVal);
            int r = std::get<int>(rightVal);

            if (bin->op == "+") return l + r;
            if (bin->op == "-") return l - r;
            if (bin->op == "*") return l * r;
            if (bin->op == "/") {
                if (r == 0) throw std::runtime_error("Runtime Error: Division by zero");
                return l / r;
            }
            if (bin->op == "==") return (l == r) ? 1 : 0;
            if (bin->op == "!=") return (l != r) ? 1 : 0;
            if (bin->op == "<")  return (l < r)  ? 1 : 0;
            if (bin->op == ">")  return (l > r)  ? 1 : 0;
            if (bin->op == "<=") return (l <= r) ? 1 : 0;
            if (bin->op == ">=") return (l >= r) ? 1 : 0;
        }
    }

    return 0;
}

bool Evaluator::isTruthy(const Value& val) const {
    if (std::holds_alternative<int>(val)) {
        return std::get<int>(val) != 0;
    }
    return !std::get<std::string>(val).empty();
}

void Evaluator::printRuntimeState() const {
    std::cout << std::left << std::setw(15) << "Variable" << "Value\n";
    std::cout << "----------------------------------------\n";
    for (const auto& [name, val] : environment) {
        std::cout << std::left << std::setw(15) << name;
        printValue(val);
        std::cout << "\n";
    }
}

void Evaluator::printValue(const Value& val) const {
    if (std::holds_alternative<int>(val)) {
        std::cout << std::get<int>(val);
    } else {
        std::cout << "\"" << std::get<std::string>(val) << "\"";
    }
}
