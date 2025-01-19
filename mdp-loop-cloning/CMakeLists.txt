cmake_minimum_required(VERSION 3.29)

# Set target architecture
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Specify the cross-compilation settings
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_C_COMPILER_TARGET "aarch64-linux-gnu")
set(CMAKE_CXX_COMPILER_TARGET "aarch64-linux-gnu")

# Set target-specific flags
add_compile_options(-march=v8a)

project(mdp-loop-cloning)

# Require C++23 and disable extensions for all targets 
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/main.cpp
    src/microbenchmark.cpp
)

# Add source to this project's executable.
add_executable(MdpLoopCloning ${SOURCES})

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
target_include_directories(MdpLoopCloning PRIVATE ${SRC_DIR})
target_link_libraries(MdpLoopCloning -static)
