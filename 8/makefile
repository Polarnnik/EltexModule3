# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = main
SRCS = main.c

# Default rule to build the program
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Clean up the generated files
clean:
	rm -f $(TARGET)

# Phony targets to prevent conflicts with files
.PHONY: all clean
