CC = gcc
CFLAGS = -Wall -Wextra -I./include
SRC_DIR = src
OBJ_DIR = obj

# Get all .c files from src directory
SRC = $(wildcard $(SRC_DIR)/*.c)
# Generate object file names
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
# Final executable name
TARGET = main

# Create necessary directories
$(shell mkdir -p $(OBJ_DIR))

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean 