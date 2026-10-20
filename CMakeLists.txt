cmake_minimum_required(VERSION 3.12)

project(toolsday LANGUAGES CXX)

# Use modern C++ (adjust if needed)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debug flags for GDB.
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

# Collect all .cpp files from src/
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)

include_directories(inc)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include headers (if you have include/ or headers in src/)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

# Install target to bin/
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/bin)