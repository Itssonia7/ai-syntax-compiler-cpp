#include "GeminiClient.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

GeminiClient::GeminiClient(std::string apiKey) : apiKey(std::move(apiKey)) {}

size_t GeminiClient::writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* response = static_cast<std::string*>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string GeminiClient::fetchDiagnosticAnalysis(const std::string& promptPayload) {
    if (apiKey.empty()) {
        return "[Gemini API Error] API key is missing. Please set the GEMINI_API_KEY environment variable.";
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        return "[Gemini API Error] Failed to initialize CURL.";
    }

    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=" + apiKey;
    std::string responseBuffer;

    // Construct JSON Payload
    json requestBody = {
        {"contents", json::array({
            {
                {"parts", json::array({
                    {{"text", promptPayload}}
                })}
            }
        })}
    };

    std::string jsonStr = requestBody.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return std::string("[Gemini API Error] Network request failed: ") + curl_easy_strerror(res);
    }

    try {
        json responseJson = json::parse(responseBuffer);
        
        if (responseJson.contains("error")) {
            return "[Gemini API Error] " + responseJson["error"]["message"].get<std::string>();
        }

        if (responseJson.contains("candidates") && 
            !responseJson["candidates"].empty() &&
            responseJson["candidates"][0].contains("content") &&
            responseJson["candidates"][0]["content"].contains("parts")) {
            
            return responseJson["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();
        }
    } catch (const std::exception& e) {
        return std::string("[Gemini API Error] Failed to parse JSON response: ") + e.what();
    }

    return "[Gemini API Error] Unexpected response format from Gemini API.";
}
