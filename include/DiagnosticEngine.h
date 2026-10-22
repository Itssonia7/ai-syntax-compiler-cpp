#ifndef DIAGNOSTIC_ENGINE_H
#define DIAGNOSTIC_ENGINE_H

#include <string>
#include <vector>
#include <iostream>

enum class DiagnosticLevel {
    ERROR,
    WARNING,
    INFO
};

struct Diagnostic {
    DiagnosticLevel level;
    std::string phase; // "Lexer", "Parser", or "Semantic"
    int line;
    int column;
    std::string message;
    std::string codeSnippet;
};

class DiagnosticEngine {
public:
    void report(DiagnosticLevel level, const std::string& phase, int line, int column, 
                const std::string& message, const std::string& sourceCode);
    
    bool hasErrors() const;
    void printDiagnostics() const;
    std::string generateAIPrompt() const;
    
    const std::vector<Diagnostic>& getDiagnostics() const { return diagnostics; }

private:
    std::vector<Diagnostic> diagnostics;
    std::string extractLineContext(const std::string& source, int line) const;
};

#endif // DIAGNOSTIC_ENGINE_H
