FROM ubuntu:22.04

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    ninja-build \
    mingw-w64 \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set up environment variables
ENV CC=x86_64-w64-mingw32-gcc
ENV CXX=x86_64-w64-mingw32-g++
ENV AR=x86_64-w64-mingw32-ar
ENV RANLIB=x86_64-w64-mingw32-ranlib
ENV STRIP=x86_64-w64-mingw32-strip
ENV CMAKE_TOOLCHAIN_FILE=/usr/share/mingw-w64/toolchain-x86_64-w64-mingw32.cmake

# Set working directory
WORKDIR /build

# Copy source files
COPY . .

# Build command
CMD ["bash", "-c", "mkdir -p build && cd build && cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build ."] 