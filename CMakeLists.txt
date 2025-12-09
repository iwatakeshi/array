cmake_minimum_required(VERSION 3.10)
# Include FetchContent module
include(FetchContent)

# Project name and version
project(Array VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)
include_directories(lib/fmt/include)

# Check for Catch2
find_package(Catch2 3.7.1 QUIET)
if (NOT Catch2_FOUND)
  # Fetch Catch2
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1  # Specify the desired version
  )
  FetchContent_MakeAvailable(Catch2)
endif()

# Check for fmt
find_package(fmt 8.0.1 QUIET)
if (NOT fmt_FOUND)
  # Fetch fmt
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 8.0.1  # Specify the desired version
  )
  FetchContent_MakeAvailable(fmt)
endif()

# Source files
file(GLOB SRC_FILES src/*.cpp)
file(GLOB TEST_FILES test/*.cpp)

# Add the library target for the public interface
add_library(array INTERFACE)
target_include_directories(array INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(array INTERFACE cxx_std_23)
target_link_libraries(array INTERFACE fmt::fmt)

# Add the executable target for the main application (for testing purposes)
add_executable(main ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(main PRIVATE fmt::fmt)

# Add the test executable
add_executable(test_runner ${TEST_FILES})
target_link_libraries(test_runner PRIVATE Catch2::Catch2WithMain fmt::fmt)

# Enable CTest
include(CTest)
include(Catch)
catch_discover_tests(test_runner)

# Install the library headers
install(DIRECTORY include/ DESTINATION include)