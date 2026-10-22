#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>

struct Symbol {
    std::string name;
    std::string type;
    int lineDeclared;
};

class SymbolTable {
public:
    bool insert(const std::string& name, const std::string& type, int line) {
        if (table.find(name) != table.end()) {
            return false; // Duplicate symbol declaration error
        }
        table[name] = Symbol{name, type, line};
        return true;
    }

    Symbol* lookup(const std::string& name) {
        auto it = table.find(name);
        if (it != table.end()) {
            return &it->second;
        }
        return nullptr;
    }

    void print() const {
        std::cout << std::left << std::setw(15) << "Identifier" 
                  << std::setw(12) << "Type" 
                  << std::setw(10) << "Declared Line" << "\n";
        std::cout << "----------------------------------------\n";
        for (const auto& [name, sym] : table) {
            std::cout << std::left << std::setw(15) << sym.name 
                      << std::setw(12) << sym.type 
                      << std::setw(10) << sym.lineDeclared << "\n";
        }
    }

private:
    std::unordered_map<std::string, Symbol> table;
};

#endif // SYMBOL_TABLE_H
