cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Array VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)
include_directories(lib/fmt/include)

# Find the Catch2 package
find_package(Catch2 REQUIRED)

# Find the fmt package
find_package(fmt REQUIRED)

# Source files
file(GLOB SRC_FILES src/*.cpp)
file(GLOB TEST_FILES test/*.cpp)
set(SOURCES ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Add the executable target for the main application
add_executable(main ${SOURCES})

# Link fmt to the main executable
target_link_libraries(main PRIVATE fmt::fmt)

# Add the test executable
add_executable(test_runner ${TEST_FILES})

# Link Catch2 to the test_runner executable
target_link_libraries(test_runner PRIVATE Catch2::Catch2WithMain)

# Enable CTest
include(CTest)
include(Catch)

# Automatically discover and register tests
catch_discover_tests(test_runner)

# Optionally, you can set up custom options for Catch2 tests here
# For example:
set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE POST_BUILD) # Change discovery mode if needed