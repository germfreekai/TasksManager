# compiler
CC = gcc

# Directories
SRC = src
OBJ = obj
OBJ_DEBUG = obj_debug
BINDIR = bin
FUNCTIONS_SRC = $(SRC)/tm_functions_c

# Source files
SRCS = $(wildcard $(SRC)/*.c)
SRCS_H = $(wildcard $(FUNCTIONS_SRC)/*.c)

# Object files
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
OBJS_H = $(patsubst $(FUNCTIONS_SRC)/%.c, $(OBJ)/%.o, $(SRCS_H))

# Debug object files
OBJS_DEBUG = $(patsubst $(SRC)/%.c, $(OBJ_DEBUG)/%.o, $(SRCS))
OBJS_H_DEBUG = $(patsubst $(FUNCTIONS_SRC)/%.c, $(OBJ_DEBUG)/%.o, $(SRCS_H))

# Binaries
BIN = $(BINDIR)/tasksmanager
VALGRIND_BIN = $(BINDIR)/tasksmanager.out
DEBUG_BIN = $(BINDIR)/tasksmanager_debug.out

# Targets
all: $(BIN)

# Compile
$(BIN): $(OBJS) $(OBJS_H)
	$(CC) $(OBJS) $(OBJS_H) -o $(BIN)

valgrind: $(OBJS) $(OBJS_H)
	$(CC) $(OBJS) $(OBJS_H) -o $(VALGRIND_BIN)

debug: $(OBJS_DEBUG) $(OBJS_H_DEBUG)
	$(CC) $(OBJS_DEBUG) $(OBJS_H_DEBUG) -g -o $(DEBUG_BIN)

# Create obj files
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $< -o $@
$(OBJ)/%.o: $(FUNCTIONS_SRC)/%.c
	$(CC) -c $< -o $@

# Create debug obj files
$(OBJ_DEBUG)/%.o: $(SRC)/%.c
	$(CC) -c $< -g -o $@
$(OBJ_DEBUG)/%.o: $(FUNCTIONS_SRC)/%.c
	$(CC) -c $< -g -o $@

# Clean
clean:
	rm -f $(OBJ)/* $(OBJ_DEBUG)/* $(BINDIR)/*
