CC = gcc
CFLAGS = -Wall -Wextra -I./include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SERVER_SRC = $(SRC_DIR)/server.c $(SRC_DIR)/http.c
CLIENT_SRC = $(SRC_DIR)/client.c $(SRC_DIR)/http.c

SERVER_OBJ = $(SERVER_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CLIENT_OBJ = $(CLIENT_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

SERVER_TARGET = $(BIN_DIR)/server
CLIENT_TARGET = $(BIN_DIR)/client

# Create necessary directories
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Default target
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Server executable
$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Client executable
$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
