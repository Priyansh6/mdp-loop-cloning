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

# Common sources used by all executables
set(Common_SOURCES
    src/Utils.cpp
)

# List of executables to process
set(EXECUTABLES
    SimpleSS
    DependenceDistanceSS
)

# Define source files for each executable separately, 
# ensuring the name is of the form ${EXEC}_SOURCES
set(SimpleSS_SOURCES
    src/SimpleSS/Main.cpp
    src/SimpleSS/Variants.cpp
)
set(DependenceDistanceSS_SOURCES
    src/DependenceDistanceSS/Main.cpp
    src/DependenceDistanceSS/Variants.cpp
)

# Source directory
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# Loop through executables to create targets
foreach(EXEC ${EXECUTABLES})
    # Use the manually defined sources for each executable
    # The ${${EXEC}_SOURCES} syntax references variables like SimpleSS_SOURCES
    add_executable(${EXEC} ${${EXEC}_SOURCES} ${Common_SOURCES})
    target_include_directories(${EXEC} PRIVATE ${SRC_DIR})
    target_link_libraries(${EXEC} -static)
endforeach()