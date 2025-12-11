# COMPILER + FLAGS
GCC    := gcc                  # compiler CLI
FLAGS  := -Wall -Wextra -Iinc  # compiling flags : common warnings, detailed warnings, searchpath for includes                   

# PROJECT DIRECTORIES
SRC_DIR := src
BUILD_DIR := tmpbuild

# SRC + OBJECTS
SRCS := $(wildcard $(SRC_DIR)/*.c)               # all .c files
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)  # all .o files (machine  code) in tmpbuild

# OUTPUT EXEC
TARGET := enseash


all: $(TARGET)

# enseash as the exec of all .o object from compile d.c and .h
$(TARGET): $(OBJS)
	$(GCC) $(FLAGS) -o $@ $^

# BUILD .o FILES in temporary build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(GCC) $(FLAGS) -c $< -o $@

# CLEAN TMPBUILD FILES
clean:
	rm -rf $(BUILD_DIR)/*

# CLEAN EXECUTABLE
fclean: clean
	rm -f $(TARGET)

# REBUILD
re: fclean all
