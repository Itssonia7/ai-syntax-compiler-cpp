#include "DiagnosticEngine.h"
#include <sstream>
#include <iomanip>

void DiagnosticEngine::report(DiagnosticLevel level, const std::string& phase, int line, int column, 
                               const std::string& message, const std::string& sourceCode) {
    std::string snippet = extractLineContext(sourceCode, line);
    diagnostics.push_back({level, phase, line, column, message, snippet});
}

bool DiagnosticEngine::hasErrors() const {
    for (const auto& diag : diagnostics) {
        if (diag.level == DiagnosticLevel::ERROR) return true;
    }
    return false;
}

std::string DiagnosticEngine::extractLineContext(const std::string& source, int targetLine) const {
    std::stringstream ss(source);
    std::string line;
    int currentLine = 1;
    while (std::getline(ss, line)) {
        if (currentLine == targetLine) {
            return line;
        }
        currentLine++;
    }
    return "";
}

void DiagnosticEngine::printDiagnostics() const {
    if (diagnostics.empty()) {
        std::cout << "No diagnostics reported.\n";
        return;
    }

    for (const auto& diag : diagnostics) {
        std::string levelStr = (diag.level == DiagnosticLevel::ERROR) ? "[ERROR]" :
                               (diag.level == DiagnosticLevel::WARNING) ? "[WARNING]" : "[INFO]";
        
        std::cout << levelStr << " Phase: " << diag.phase 
                  << " | Line " << diag.line << ", Col " << diag.column << "\n"
                  << "  Message: " << diag.message << "\n";
        if (!diag.codeSnippet.empty()) {
            std::cout << "  Code:    " << diag.codeSnippet << "\n";
        }
        std::cout << "\n";
    }
}

std::string DiagnosticEngine::generateAIPrompt() const {
    if (diagnostics.empty()) return "";

    std::stringstream prompt;
    prompt << "You are an expert compiler diagnostic AI assistant. Analyze the following compiler diagnostic report and provide:\n"
           << "1. A concise explanation of why the syntax or semantic error occurred.\n"
           << "2. A concrete, minimal code fix.\n"
           << "3. Learning advice to prevent similar syntax issues.\n\n"
           << "=== DIAGNOSTIC REPORT ===\n";

    for (const auto& diag : diagnostics) {
        prompt << "- Phase: " << diag.phase << "\n"
               << "  Line: " << diag.line << ", Column: " << diag.column << "\n"
               << "  Error Message: " << diag.message << "\n"
               << "  Code Context: \"" << diag.codeSnippet << "\"\n\n";
    }

    return prompt.str();
}
