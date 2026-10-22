FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install GCC, CMake, libcurl, nlohmann-json, and Python
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    make \
    libcurl4-openssl-dev \
    nlohmann-json3-dev \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app

# Build C++ compiler binary during image creation
RUN mkdir -p build && cd build && cmake .. && make

# Install Python backend dependencies
RUN pip3 install --no-cache-dir fastapi uvicorn pydantic

EXPOSE 8000

CMD ["python3", "app.py"]
