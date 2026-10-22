#ifndef GEMINI_CLIENT_H
#define GEMINI_CLIENT_H

#include <string>

class GeminiClient {
public:
    explicit GeminiClient(std::string apiKey);
    std::string fetchDiagnosticAnalysis(const std::string& promptPayload);

private:
    std::string apiKey;
    static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
};

#endif // GEMINI_CLIENT_H
