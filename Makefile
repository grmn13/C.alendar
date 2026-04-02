# Compiler and Flags
CC      := gcc
CFLAGS  := -Wall -Wextra -g -Iinclude
LDFLAGS := -lncurses -lcjson

# Target Binary Name
TARGET  := calendar_app

# Directories
SRCDIR  := src
OBJDIR  := obj

# Logic to find all .c files and convert them to .o filenames
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Default Rule
all: $(TARGET)

# Link the final binary
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile .c files to .o files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up build files
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean
