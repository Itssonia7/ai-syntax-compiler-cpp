#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include <iostream>
#include <iomanip>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD_INT:     return "KEYWORD_INT";
        case TokenType::KEYWORD_STRING:  return "KEYWORD_STRING";
        case TokenType::KEYWORD_BOOLEAN: return "KEYWORD_BOOLEAN";
        case TokenType::IDENTIFIER:      return "IDENTIFIER";
        case TokenType::INT_LITERAL:     return "INT_LITERAL";
        case TokenType::STRING_LITERAL:  return "STRING_LITERAL";
        case TokenType::ASSIGN:          return "ASSIGN";
        case TokenType::SEMICOLON:       return "SEMICOLON";
        case TokenType::END_OF_FILE:     return "END_OF_FILE";
        default:                         return "UNKNOWN";
    }
}

int main() {
    std::string source = "int score = 100;\nstring name = \"compiler\";";

    std::cout << "========================================\n";
    std::cout << "          COMPILER PIPELINE             \n";
    std::cout << "========================================\n\n";

    // STAGE 1: LEXICAL ANALYSIS
    std::cout << "[STAGE 1] Lexical Analysis (Token Stream)\n";
    std::cout << "----------------------------------------\n";
    
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenizeAll();

    for (const auto& tok : tokens) {
        std::cout << "Line " << std::setw(2) << tok.line 
                  << ", Col " << std::setw(2) << tok.column 
                  << " | " << std::setw(15) << std::left << tokenTypeToString(tok.type)
                  << " | Value: \"" << tok.value << "\"\n";
    }
    std::cout << "\n";

    // STAGE 2: PARSING & AST GENERATION
    std::cout << "[STAGE 2] Parsing & Abstract Syntax Tree (AST)\n";
    std::cout << "----------------------------------------\n";
    
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parseProgram();
    if (ast) {
        ast->print();
    }
    std::cout << "\n";

    // STAGE 3: SEMANTIC ANALYSIS & SYMBOL TABLE
    std::cout << "[STAGE 3] Semantic Analysis & Symbol Table\n";
    std::cout << "----------------------------------------\n";
    
    SemanticAnalyzer analyzer;
    bool success = analyzer.analyze(*ast);

    std::cout << "Symbol Table:\n";
    analyzer.getSymbolTable().print();

    if (!success) {
        std::cout << "\nSemantic Errors Encountered:\n";
        for (const auto& err : analyzer.getErrors()) {
            std::cout << " - " << err << "\n";
        }
    } else {
        std::cout << "\nSemantic Analysis Passed: 0 Errors Found.\n";
    }
    std::cout << "\n";

    return 0;
}
