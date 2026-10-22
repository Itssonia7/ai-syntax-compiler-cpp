# AI-Powered C++ Compiler & Diagnostic Pipeline

A 5-stage toy C++ compiler pipeline built from scratch using C++17. It takes custom C++-like code, tokenizes it, generates an Abstract Syntax Tree (AST), performs type checking, calls the Gemini 2.5 Flash API for live AI diagnostic fixes whenever errors occur, and executes valid code in an interpreter runtime.

---

## Key Features

- **5-Stage Pipeline Architecture:** Clean separation of concerns from tokenization to execution.
- **Precedence-Based Parsing:** Uses recursive descent with operator precedence climbing for arithmetic (`+`, `-`, `*`, `/`) and comparisons (`==`, `!=`, `<`, `>`, `<=`, `>=`).
- **Control Flow Support:** Handles `if`/`else` conditions, `while` loops, and `{ ... }` block scoping.
- **Static Type Checking:** Catches redeclarations, undeclared variables, and type mismatches before execution.
- **Live Gemini AI Integration:** Automatically extracts error context, builds a structured diagnostic prompt, and streams fix recommendations from Gemini directly in your terminal via `libcurl`.
- **AST Runtime Interpreter:** Evaluates valid AST nodes and tracks variable state inside an environment table.
- **Secret Management:** Supports loading API keys safely from a `.env.local` file.

---

## Pipeline Architecture

```
             +-----------------------+
             |    Raw Source Code    |
             +-----------+-----------+
                         |
                         v
             +-----------------------+
             |   Stage 1: Lexer      | ---> Token Stream
             +-----------+-----------+
                         |
                         v
             +-----------------------+
             |   Stage 2: Parser     | ---> Abstract Syntax Tree (AST)
             +-----------+-----------+
                         |
                         v
             +-----------------------+
             | Stage 3: Semantic     | ---> Symbol Table & Type Check
             +-----------+-----------+
                         |
               +---------+---------+
               |                   |
    (Errors Found)         (0 Errors Found)
               |                   |
               v                   v
 +-------------------+   +-------------------+
 | Stage 4: AI Engine|   | Stage 5: Evaluator|
 | (Gemini API Fix)  |   | (Runtime Exec)    |
 +-------------------+   +-------------------+

```

1. **Stage 1: Lexical Analysis (`Lexer`)**  
   Converts source code into a stream of typed tokens (`INT_LITERAL`, `IDENTIFIER`, `PLUS`, `KEYWORD_IF`, etc.) with line and column position tracking.
2. **Stage 2: Parsing & AST (`Parser`)**  
   Constructs the AST using recursive descent parsing. Handles operator precedence and nested statement blocks.
3. **Stage 3: Semantic Analysis (`SemanticAnalyzer`)**  
   Populates the symbol table, verifies scope, and ensures typed expressions match variable definitions.
4. **Stage 4: Diagnostic Bridge (`DiagnosticEngine` & `GeminiClient`)**  
   If syntax or type errors are found, extracts surrounding source code snippets and sends an HTTP POST request via `libcurl` to the Gemini API for live terminal fixes.
5. **Stage 5: Runtime Interpreter (`Evaluator`)**  
   If no errors are found, walks the AST and executes variable declarations, loop iterations, conditional branches, and arithmetic directly.

## Tech Stack

- **Language:** C++17
- **Build System:** CMake (3.14+)
- **HTTP Client:** `libcurl`
- **JSON Processing:** `nlohmann-json`
- **AI Model:** Google Gemini 2.5 Flash

---

## Project Structure

```text
.
├── CMakeLists.txt          # CMake build config
├── .env.local              # Local environment variables (GEMINI_API_KEY)
├── include/
│   ├── AST.h               # AST Node definitions
│   ├── DiagnosticEngine.h  # Context extraction & AI prompt generation
│   ├── EnvLoader.h         # Zero-dependency .env.local loader
│   ├── Evaluator.h         # AST runtime interpreter & environment
│   ├── GeminiClient.h      # libcurl wrapper for Gemini API
│   ├── Lexer.h             # Lexer class declaration
│   ├── Parser.h            # Parser class declaration
│   ├── SemanticAnalyzer.h  # Semantic analysis & type inference
│   ├── SymbolTable.h       # Variable symbol table structure
│   └── Token.h             # Token enum and struct definitions
└── src/
    ├── DiagnosticEngine.cpp
    ├── Evaluator.cpp
    ├── GeminiClient.cpp
    ├── Lexer.cpp
    ├── Main.cpp             # Pipeline runner
    ├── Parser.cpp
    └── SemanticAnalyzer.cpp

```

---

## Getting Started

### Prerequisites

For Ubuntu / Linux distributions:

```bash
sudo apt update
sudo apt install -y build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev

```

### Installation & Setup

1. **Clone the repository:**

```bash
git clone [https://github.com/your-username/ai-syntax-compiler-cpp.git](https://github.com/your-username/ai-syntax-compiler-cpp.git)
cd ai-syntax-compiler-cpp

```

2. **Configure your API Key:**
   Create a `.env.local` file in the project root:

```bash
echo "GEMINI_API_KEY=your_actual_gemini_api_key_here" > .env.local

```

3. **Build the compiler:**

```bash
mkdir build && cd build
cmake ..
make

```

4. **Run the pipeline:**

```bash
./compiler

```

---

## Sample Execution Output

### Valid Source Code (Execution Run)

Input:

```cpp
int score = 100 + 50 * 2;
int counter = 0;
while (counter < 3) {
    counter = counter + 1;
    score = score - 10;
}

```

Output:

========================================
RUNNING PIPELINE: Full Execution Test
========================================

## [STAGE 1] Lexical Analysis (Token Stream)

Line 1, Col 1 | KEYWORD_INT | Value: "int"
Line 1, Col 5 | IDENTIFIER | Value: "score"
Line 1, Col 11 | ASSIGN | Value: "="
Line 1, Col 13 | INT_LITERAL | Value: "100"
Line 1, Col 17 | PLUS | Value: "+"
...

## [STAGE 2] Parsing & Abstract Syntax Tree (AST)

ProgramNode
VarDeclNode (int score):
BinaryExprNode (+):
LiteralNode (int): 100
BinaryExprNode (\*):
LiteralNode (int): 50
LiteralNode (int): 2
...

## [STAGE 3] Semantic Analysis & Symbol Table

Symbol Table:
Identifier Type Declared Line

---

score int 1  
counter int 1

Semantic Analysis Passed: 0 Errors Found.

## [STAGE 4] Diagnostics & Live Gemini AI Diagnosis

No errors detected.

## [STAGE 5] AST Execution & Runtime Environment

Execution Output / Final State:
Variable Value

---

score 170
counter 3

---

## Future Improvements

- [ ] Add support for user-defined functions and call stack frames.
- [ ] Implement an interactive REPL mode (`--repl`).
- [ ] Add unit testing coverage with GoogleTest (`gtest`).
- [ ] Emit low-level assembly or LLVM Intermediate Representation (IR).

---

## License

Distributed under the MIT License. See `LICENSE` for details.
