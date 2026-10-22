#include "Lexer.h"
#include <iostream>

int main() {
    std::string source = "int score = 100;\nstring name = \"compiler\";";
    
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenizeAll();

    std::cout << "--- TOKENIZER TEST OUTPUT ---\n";
    for (const auto& tok : tokens) {
        std::cout << "Line " << tok.line << ", Col " << tok.column 
                  << " | Type: " << static_cast<int>(tok.type) 
                  << " | Value: \"" << tok.value << "\"\n";
    }

    return 0;
}
