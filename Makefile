# ==============================
# Settings
# ==============================
CC        ?= gcc
CFLAGS    := -std=c17 -Wall -Wextra -O2 -Iinclude
LDLIBS    := -lncurses -lm
SRC_DIR   := src
BUILD_DIR := build
TARGET    := matrix_timer
# ==============================
# Files
# ==============================
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
# ==============================
# Rules
# ==============================
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
