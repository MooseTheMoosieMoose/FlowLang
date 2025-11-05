cmake_minimum_required(VERSION 3.16)
project(Flow LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect all .cpp files in src/
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Define the executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Optionally set build folder behavior
# (You don't need to specify "build" here — just run CMake from it)
# Example usage from terminal:
#   mkdir build
#   cd build
#   cmake ..
#   cmake --build .