#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "DiagnosticEngine.h"
#include "GeminiClient.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

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

void runCompilerPipeline(const std::string& sourceCode, const std::string& testName) {
    std::cout << "========================================\n";
    std::cout << "  RUNNING PIPELINE: " << testName << "\n";
    std::cout << "========================================\n\n";

    DiagnosticEngine diagnostics;

    // STAGE 1: LEXICAL ANALYSIS
    std::cout << "[STAGE 1] Lexical Analysis (Token Stream)\n";
    std::cout << "----------------------------------------\n";
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenizeAll();

    for (const auto& tok : tokens) {
        if (tok.type == TokenType::UNKNOWN) {
            diagnostics.report(DiagnosticLevel::ERROR, "Lexer", tok.line, tok.column,
                               "Unrecognized token '" + tok.value + "'", sourceCode);
        }
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
    std::unique_ptr<ProgramNode> ast = nullptr;
    try {
        ast = parser.parseProgram();
        if (ast) ast->print();
    } catch (const std::exception& e) {
        diagnostics.report(DiagnosticLevel::ERROR, "Parser", 1, 1, e.what(), sourceCode);
    }
    std::cout << "\n";

    // STAGE 3: SEMANTIC ANALYSIS
    std::cout << "[STAGE 3] Semantic Analysis & Symbol Table\n";
    std::cout << "----------------------------------------\n";
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
    std::cout << "[STAGE 4] Diagnostics & Live Gemini AI Diagnosis\n";
    std::cout << "----------------------------------------\n";
    if (diagnostics.hasErrors()) {
        diagnostics.printDiagnostics();
        
        std::string promptPayload = diagnostics.generateAIPrompt();
        std::cout << "--- SENDING PROMPT TO GEMINI API ---\n";

        const char* envApiKey = std::getenv("GEMINI_API_KEY");
        std::string apiKey = envApiKey ? std::string(envApiKey) : "";

        if (apiKey.empty()) {
            std::cout << "[Note] GEMINI_API_KEY environment variable not set.\n";
            std::cout << "Export your key using: export GEMINI_API_KEY=\"your_key_here\"\n\n";
            std::cout << "Generated Prompt Payload:\n" << promptPayload << "\n";
        } else {
            GeminiClient client(apiKey);
            std::string aiResponse = client.fetchDiagnosticAnalysis(promptPayload);
            std::cout << "\n--- GEMINI AI DIAGNOSTIC ADVICE ---\n";
            std::cout << aiResponse << "\n\n";
        }
    } else {
        std::cout << "No errors detected. Compiler pipeline completed successfully.\n\n";
    }
}

int main() {
    std::string errorSource = "int score = 100;\nscore = \"invalid_type_assignment\";";
    runCompilerPipeline(errorSource, "Semantic Error Live AI Diagnostic Test");

    return 0;
}
