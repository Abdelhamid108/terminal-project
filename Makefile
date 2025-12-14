# Makefile for Custom Shell Project
# @author Abdelhamid
# @date 2025-12-14

# Compiler to use
CC=gcc

# Compiler flags
# -Wall: Enable all warnings
# -g: Add debug information
CFLAGS=-Wall -g

# Header files dependency
DEPS = shell.h

# Object files to build
OBJ = main.o utils.o parser.o builtins.o executor.o history.o

# Rule to compile .c files to .o object files
# $@: The target (e.g., main.o)
# $<: The first dependency (e.g., main.c)
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule to link object files into the final executable
# $^: All dependencies (the object files)
myshell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Clean up build artifacts
# Remove object files and the executable
clean:
	rm -f *.o myshell
