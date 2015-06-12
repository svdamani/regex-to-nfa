# Makefile for compiling and linking the project
INCLUDE_PATH := ./src
SOURCE_PATH  := ./src
OBJECT_PATH  := ./obj
PROGRAM := generate_nfa

# Flags for GCC
CFLAGS  = -Wall -std=c99
LDFLAGS = -O3

vpath %.h $(INCLUDE_PATH)
vpath %.c $(SOURCE_PATH)
vpath %.o $(OBJECT_PATH)

OBJECT_PATH ?= obj
PROGRAM ?= run

# List of source files
source-list = $(notdir $(wildcard $(addsuffix /*.c, $(SOURCE_PATH))))

# List of object files
object-list = $(addprefix $(OBJECT_PATH)/, $(subst .c,.o, $(source-list)))

# Generating object files from source
$(OBJECT_PATH)/%.o: %.c
	@mkdir -p $(OBJECT_PATH)
	@$(COMPILE.c) $(OUTPUT_OPTION) $<

# Linking object files to build executable
$(PROGRAM): $(object-list)
	@$(LINK.c) $^ -o $@

# Target to clean all generated files
clean:
	@$(RM) -r $(OBJECT_PATH)/ *.dot $(PROGRAM)