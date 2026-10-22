#ifndef ENV_LOADER_H
#define ENV_LOADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

inline void loadEnv(const std::string& primaryFile = ".env.local") {
    std::ifstream file(primaryFile);
    if (!file.is_open()) {
        file.open(".env"); // Fallback to .env if .env.local isn't present
        if (!file.is_open()) return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);

            // Trim whitespace and quotes
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);

            value.erase(0, value.find_first_not_of(" \t\r\n\"'"));
            value.erase(value.find_last_not_of(" \t\r\n\"'") + 1);

            // Only set if not already set in the system environment
            if (std::getenv(key.c_str()) == nullptr) {
                setenv(key.c_str(), value.c_str(), 1);
            }
        }
    }
}

#endif // ENV_LOADER_H
