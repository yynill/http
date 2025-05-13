CC = gcc
CFLAGS = -Wall -Wextra -I./include
SRC_DIR = src
OBJ_DIR = obj

# Source files
SERVER_SRC = $(SRC_DIR)/server.c
CLIENT_SRC = $(SRC_DIR)/client.c

# Object files
SERVER_OBJ = $(SERVER_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CLIENT_OBJ = $(CLIENT_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Targets
SERVER_TARGET = server
CLIENT_TARGET = client

# Create necessary directories
$(shell mkdir -p $(OBJ_DIR))

# Default target: build both
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Build server executable
$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Build client executable
$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(SERVER_TARGET) $(CLIENT_TARGET)

.PHONY: all clean
