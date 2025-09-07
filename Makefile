# Compiler + Flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./src -w

# Output directory
OUTDIR = out
LIB = $(OUTDIR)/libprm.a

# Sources
SRC = src/prm.c src/backend/opengl_es_backend.c src/backend/vulkan_backend.c
OBJ = $(SRC:.c=.o)

# Default target
all: $(LIB)

# Build library
$(LIB): $(OBJ)
	@mkdir -p $(OUTDIR)
	ar rcs $@ $^

# Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJ) $(OUTDIR)

# Example test build
test: all
	$(CC) $(CFLAGS) tests/test1.c -L$(OUTDIR) -lprm -lGLESv2 -lEGL -o $(OUTDIR)/test1_prm

.PHONY: all clean test