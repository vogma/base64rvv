cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(base64rvv VERSION 0.1 LANGUAGES C CXX)

set(BUILD_EXAMPLE CACHE BOOL OFF)

ADD_COMPILE_OPTIONS(-march=rv64gcv -mabi=lp64d -O3 -static -Wall -g)

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
if(BUILD_EXAMPLE)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    include_directories(${PROJECT_SOURCE_DIR}/example/include)
    add_executable(base64 ${PROJECT_SOURCE_DIR}/example/src/base64.cpp)
    target_link_libraries(base64 b64rvv -static)
endif()