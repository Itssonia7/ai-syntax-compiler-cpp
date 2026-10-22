#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "DiagnosticEngine.h"
#include "GeminiClient.h"
#include "Evaluator.h"
#include "EnvLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD_INT:     return "KEYWORD_INT";
        case TokenType::KEYWORD_STRING:  return "KEYWORD_STRING";
        case TokenType::KEYWORD_BOOLEAN: return "KEYWORD_BOOLEAN";
        case TokenType::KEYWORD_IF:      return "KEYWORD_IF";
        case TokenType::KEYWORD_ELSE:    return "KEYWORD_ELSE";
        case TokenType::KEYWORD_WHILE:   return "KEYWORD_WHILE";
        case TokenType::IDENTIFIER:      return "IDENTIFIER";
        case TokenType::INT_LITERAL:     return "INT_LITERAL";
        case TokenType::STRING_LITERAL:  return "STRING_LITERAL";
        case TokenType::ASSIGN:          return "ASSIGN";
        case TokenType::PLUS:            return "PLUS";
        case TokenType::MINUS:           return "MINUS";
        case TokenType::STAR:            return "STAR";
        case TokenType::SLASH:           return "SLASH";
        case TokenType::EQUAL_EQUAL:     return "EQUAL_EQUAL";
        case TokenType::NOT_EQUAL:       return "NOT_EQUAL";
        case TokenType::LESS:            return "LESS";
        case TokenType::GREATER:         return "GREATER";
        case TokenType::LESS_EQUAL:      return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL:   return "GREATER_EQUAL";
        case TokenType::SEMICOLON:       return "SEMICOLON";
        case TokenType::LPAREN:          return "LPAREN";
        case TokenType::RPAREN:          return "RPAREN";
        case TokenType::LBRACE:          return "LBRACE";
        case TokenType::RBRACE:          return "RBRACE";
        case TokenType::END_OF_FILE:     return "END_OF_FILE";
        default:                         return "UNKNOWN";
    }
}

void runCompilerPipeline(const std::string& sourceCode) {
    DiagnosticEngine diagnostics;

    // STAGE 1: LEXICAL ANALYSIS
    std::cout << "[STAGE 1] Lexical Analysis\n----------------------------------------\n";
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenizeAll();

    for (const auto& tok : tokens) {
        if (tok.type == TokenType::UNKNOWN) {
            diagnostics.report(DiagnosticLevel::ERROR, "Lexer", tok.line, tok.column,
                               "Unrecognized token '" + tok.value + "'", sourceCode);
        }
    }
    std::cout << "Tokenization complete (" << tokens.size() << " tokens).\n\n";

    // STAGE 2: PARSING & AST GENERATION
    std::cout << "[STAGE 2] Parsing & AST Generation\n----------------------------------------\n";
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = nullptr;
    try {
        ast = parser.parseProgram();
        if (ast) ast->print();
    } catch (const std::exception& e) {
        diagnostics.report(DiagnosticLevel::ERROR, "Parser", 1, 1, e.what(), sourceCode);
    }
    std::cout << "\n";

    // STAGE 3: SEMANTIC ANALYSIS
    std::cout << "[STAGE 3] Semantic Analysis & Symbol Table\n----------------------------------------\n";
    if (ast) {
        SemanticAnalyzer analyzer;
        analyzer.analyze(*ast);
        analyzer.getSymbolTable().print();

        for (const auto& err : analyzer.getErrors()) {
            diagnostics.report(DiagnosticLevel::ERROR, "Semantic", 1, 1, err, sourceCode);
        }
    }
    std::cout << "\n";

    // STAGE 4: DIAGNOSTICS & LIVE AI BRIDGE
    std::cout << "[STAGE 4] Diagnostics & Gemini AI Integration\n----------------------------------------\n";
    if (diagnostics.hasErrors()) {
        diagnostics.printDiagnostics();
        
        std::string promptPayload = diagnostics.generateAIPrompt();
        const char* envApiKey = std::getenv("GEMINI_API_KEY");
        std::string apiKey = envApiKey ? std::string(envApiKey) : "";

        if (apiKey.empty() || apiKey == "your_actual_gemini_api_key_here") {
            std::cout << "[Warning] Valid GEMINI_API_KEY missing in .env.local\n";
            std::cout << "Prompt Payload Ready:\n" << promptPayload << "\n";
        } else {
            std::cout << "--> Contacting Gemini API for AI Diagnostic Analysis...\n";
            GeminiClient client(apiKey);
            std::string aiResponse = client.fetchDiagnosticAnalysis(promptPayload);
            std::cout << "\n=== GEMINI AI DIAGNOSTIC ADVICE ===\n";
            std::cout << aiResponse << "\n===================================\n\n";
        }
    } else {
        std::cout << "No syntax/semantic errors detected.\n\n";

        // STAGE 5: EXECUTION
        std::cout << "[STAGE 5] Runtime Execution\n----------------------------------------\n";
        try {
            Evaluator evaluator;
            evaluator.execute(*ast);
            std::cout << "Final Variable State:\n";
            evaluator.printRuntimeState();
        } catch (const std::exception& e) {
            std::cout << "[Runtime Error] " << e.what() << "\n";
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    loadEnv(".env.local");

    std::string sourceCode;

    if (argc > 1) {
        // Read code from input file argument
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << argv[1] << "\n";
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        sourceCode = buffer.str();
    } else {
        std::cout << "Usage: ./compiler <source_file.cpp>\n";
        std::cout << "Or paste your code below (press Ctrl+D on Unix when finished):\n";
        std::cout << "----------------------------------------\n";
        std::string line;
        while (std::getline(std::cin, line)) {
            sourceCode += line + "\n";
        }
    }

    if (!sourceCode.empty()) {
        runCompilerPipeline(sourceCode);
    }

    return 0;
}
