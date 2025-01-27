cmake_minimum_required(VERSION 3.29)

# Set target architecture
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Specify the cross-compilation settings
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_C_COMPILER_TARGET "aarch64-linux-gnu")
set(CMAKE_CXX_COMPILER_TARGET "aarch64-linux-gnu")

# Set target-specific flags
add_compile_options(-march=v8a -fno-vectorize)

project(mdp-loop-cloning)

# Require C++23 and disable extensions for all targets 
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SIMPLE_SS_SOURCES
    src/SimpleSS/Main.cpp
    src/SimpleSS/Variants.cpp
    src/utils.cpp
)

# Add source to this project's executable.
add_executable(SimpleSS ${SIMPLE_SS_SOURCES})

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
target_include_directories(SimpleSS PRIVATE ${SRC_DIR})
target_link_libraries(SimpleSS -static)
