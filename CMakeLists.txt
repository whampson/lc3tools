project(lc3tools)
cmake_minimum_required(VERSION 3.0)

# Enable more warnings
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
else()
    # Assume GCC/Clang
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Write executable to bin/ directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
file(GLOB LIB_SOURCES       "src/lib/*.c")
file(GLOB AS_SOURCES        "src/as/*.cpp")
file(GLOB EMU_SOURCES       "src/emu/*.c")

# Include directories
include_directories("include/")

# Library for shared code
add_library(lc3tools ${LIB_SOURCES})

# Executables
add_executable(lc3as ${AS_SOURCES})
add_executable(lc3emu ${EMU_SOURCES})

# Link shared code and executables
target_link_libraries(lc3as lc3tools)
target_link_libraries(lc3emu lc3tools)
