# Vectorized Base64 Encoding/Decoding for RISC-V RVV

This repository contains a vectorized Base64 encoding and decoding library targeting the RISC-V Vector Extension (RVV)

## Project Structure

- `lib/` - Contains the core library code implementing Base64 encoding and decoding using RISC-V RVV instructions.
- `src/` - Contains example executable code demonstrating how to use the Base64 library.

## Prerequisites

To build and run this project, you will need:
- A RISC-V cross-compiler, specifically `riscv64-unknown-linux-gnu-gcc`
- CMake version 3.13 or later

## Building the Project

1. **Clone the repository**
   ```bash
   git clone https://github.com/vogma/base64rvv
   cd base64rvv
   ```

2. **Create a build directory**
   ```bash
   mkdir build && cd build
   ```

3. **Generate build files using CMake**
   ```bash
   cmake ..
   ```

4. **Build the project**
   ```bash
   cmake --build .
   ```

This process will compile both the Base64 library and the example executable, which demonstrates encoding and decoding functionality.

## Cross Compilation

This project is designed to be cross-compiled for RISC-V. The `CMakeLists.txt` is configured to use the RISC-V GCC cross-compiler (`riscv64-unknown-linux-gnu-gcc`). Ensure that this compiler is available in your PATH.
