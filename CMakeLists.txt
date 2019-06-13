project(lc3tools)
cmake_minimum_required(VERSION 3.0)

# Write executable to bin/ directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
file(GLOB LIB_SOURCES       "src/lib/*.c")
file(GLOB AS_SOURCES        "src/as/*.c")
file(GLOB EMU_SOURCES       "src/emu/*.c")

# Library for shared code
add_library(lc3tools ${LIB_SOURCES})
target_include_directories(lc3tools PUBLIC include)

# Executables
add_executable(lc3as ${AS_SOURCES})
add_executable(lc3emu ${EMU_SOURCES})
target_include_directories(lc3as PRIVATE include/as)
target_include_directories(lc3emu PRIVATE include/emu)

# Link shared code and executables
target_link_libraries(lc3as lc3tools)
target_link_libraries(lc3emu lc3tools)
