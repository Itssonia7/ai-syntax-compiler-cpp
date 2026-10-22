import os
import subprocess
from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import HTMLResponse
from pydantic import BaseModel

app = FastAPI(title="AI C++ Compiler IDE")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

class CompileRequest(BaseModel):
    code: str

@app.post("/api/compile")
async def compile_code(req: CompileRequest):
    temp_file = "temp_input.cpp"
    with open(temp_file, "w") as f:
        f.write(req.code)

    binary = "./build/compiler"
    if not os.path.exists(binary):
        raise HTTPException(status_code=500, detail="Compiler binary not found. Build with make first.")

    try:
        result = subprocess.run(
            [binary, temp_file],
            capture_output=True,
            text=True,
            timeout=50
        )
        output = result.stdout if result.stdout else result.stderr
        return {"success": True, "output": output}
    except subprocess.TimeoutExpired:
        return {"success": False, "output": "Execution timed out after 50 seconds."}
    except Exception as e:
        return {"success": False, "output": str(e)}

@app.get("/", response_class=HTMLResponse)
async def serve_ide():
    if not os.path.exists("static/index.html"):
        return "<h1>Frontend index.html not found in static/ directory.</h1>"
    with open("static/index.html", "r") as f:
        return f.read()

if __name__ == "__main__":
    import uvicorn
    port = int(os.environ.get("PORT", 8000))
    uvicorn.run(app, host="0.0.0.0", port=port)
