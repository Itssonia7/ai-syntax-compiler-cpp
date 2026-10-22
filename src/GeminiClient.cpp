#include "GeminiClient.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>

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

    // List of models in order of preference (Primary -> Fallback)
    std::vector<std::string> modelCandidates = {
        "gemini-3.8-flash",
        "gemini-3.5-flash-lite",
        "gemini-3.1-flash-lite"
    };

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

    for (const auto& model : modelCandidates) {
        CURL* curl = curl_easy_init();
        if (!curl) continue;

        std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + model + ":generateContent?key=" + apiKey;
        std::string responseBuffer;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) continue;

        try {
            json responseJson = json::parse(responseBuffer);
            
            if (responseJson.contains("error")) {
                std::string errMessage = responseJson["error"]["message"].get<std::string>();
                // If high demand 503, try next candidate model
                if (errMessage.find("high demand") != std::string::npos || responseJson["error"]["code"] == 503) {
                    continue;
                }
                return "[Gemini API Error] (" + model + "): " + errMessage;
            }

            if (responseJson.contains("candidates") && 
                !responseJson["candidates"].empty() &&
                responseJson["candidates"][0].contains("content") &&
                responseJson["candidates"][0]["content"].contains("parts")) {
                
                return "[" + model + " Advice]\n" + responseJson["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();
            }
        } catch (const std::exception& e) {
            continue;
        }
    }

    return "[Gemini API Error] All candidate models (3.8-flash, 3.5-flash-lite) are currently overloaded on the free tier. Please try again in 1 minute.";
}
