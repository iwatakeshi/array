# Makefile for building the CMake project

# Variables
BUILD_DIR = build
CMAKE = cmake
MAKE = $(CMAKE) --build $(BUILD_DIR)
CTEST = ctest

# Targets
.PHONY: all build clean test run help

# Default target
all: build

# Build target
build:
	@echo "Configuring and building the project..."
	@mkdir -p $(BUILD_DIR) && $(CMAKE) -S . -B $(BUILD_DIR) && $(MAKE)

# Test target
test: build
	@echo "Running tests..."
	@cd $(BUILD_DIR) && $(CTEST)

# Run example target
run: build
	@echo "Running main example..."
	@$(BUILD_DIR)/main

# Clean target
clean:
	@echo "Cleaning the build directory..."
	@rm -rf $(BUILD_DIR)

# Help target
help:
	@echo "Makefile commands:"
	@echo "  all    - Build the project (default)"
	@echo "  build  - Configure and build the project"
	@echo "  test   - Run the tests"
	@echo "  run    - Run the main example"
	@echo "  clean  - Remove the build directory"