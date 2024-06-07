cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(base64rvv VERSION 0.1 LANGUAGES C)

# Set the RISC-V toolchain
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv)

# Specify the cross compiler
set(CMAKE_C_COMPILER riscv64-unknown-linux-gnu-gcc)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/lib/include)

# Specify source files
set(SOURCE_FILES ${PROJECT_SOURCE_DIR}/lib/src/base64_rvv_encode.c ${PROJECT_SOURCE_DIR}/lib/src/base64_rvv_decode.c)

# Define the static library to be built with the specified source files
add_library(b64rvv STATIC ${SOURCE_FILES})

#build base64 executable
include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(base64 ${PROJECT_SOURCE_DIR}/src/base64.c)
target_link_libraries(base64 b64rvv)