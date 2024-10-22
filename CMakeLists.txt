cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(base64rvv VERSION 0.1 LANGUAGES C)

# Check if the current system processor matches RISC-V
if (CMAKE_SYSTEM_PROCESSOR MATCHES "riscv")
    # Use native compiler if on a RISC-V platform
    message(STATUS "Using native compiler for RISC-V platform.")
    set(CMAKE_C_COMPILER gcc)
else()
    # Set the RISC-V toolchain for non-RISC-V platforms
    message(STATUS "Using riscv64-unknown-linux-gnu-gcc for non-RISC-V platform.")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR riscv)
    set(CMAKE_C_COMPILER riscv64-unknown-linux-gnu-gcc)
endif()

ADD_COMPILE_OPTIONS(-march=rv64imafdcv_sscofpmf_sstc_svpbmt_zicbom_zicboz_zicbop_zihintpause -mabi=lp64d -O3 -static -Wall -g)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/lib/include)

# Specify source files
set(SOURCE_FILES 
    ${PROJECT_SOURCE_DIR}/lib/src/util/utils.c 
    ${PROJECT_SOURCE_DIR}/lib/src/m1/base64_rvv_encode_m1.c 
    ${PROJECT_SOURCE_DIR}/lib/src/m1/base64_rvv_decode_m1.c
    ${PROJECT_SOURCE_DIR}/lib/src/m2/base64_rvv_decode_m2.c
    ${PROJECT_SOURCE_DIR}/lib/src/m2/base64_rvv_encode_m2.c
    ${PROJECT_SOURCE_DIR}/lib/src/m4/base64_rvv_encode_m4.c
    ${PROJECT_SOURCE_DIR}/lib/src/m4/base64_rvv_decode_m4.c
    )

# Define the static library to be built with the specified source files
add_library(b64rvv STATIC ${SOURCE_FILES})

#build base64 executable
include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(base64 ${PROJECT_SOURCE_DIR}/src/base64.c)
target_link_libraries(base64 b64rvv -static)