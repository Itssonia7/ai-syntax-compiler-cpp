cmake_minimum_required(VERSION 3.14)
project(ai_syntax_compiler_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(include)

add_executable(compiler 
    src/main.cpp 
    src/Lexer.cpp 
    src/Parser.cpp 
    src/SemanticAnalyzer.cpp 
    src/DiagnosticEngine.cpp
    src/GeminiClient.cpp
)

target_link_libraries(compiler PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
