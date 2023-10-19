# compiler
CC = gcc

# Directories
SRC = src
OBJ = obj
BINDIR = bin
HEADER_FUNCS = $(SRC)/tm_functions_c

SRCS = $(wildcard $(SRC)/*.c)
SRCS_H = $(wildcard $(HEADER_FUNCS)/*.c)

OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
OBJS_H = $(patsubst $(HEADER_FUNCS)/%.c, $(OBJ)/%.o, $(SRCS_H))

BIN = $(BINDIR)/tasksmanager
VIN = $(BINDIR)/tasksmanager.out

all: $(BIN)

# Compile
$(BIN): $(OBJS) $(OBJS_H)
	$(CC) $(OBJS) $(OBJS_H) -o $(BIN)
valgrind: $(OBJS) $(OBJS_H)
	$(CC) $(OBJS) $(OBJS_H) -o $(VIN)

# Create obj files
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $< -o $@
$(OBJ)/%.o: $(HEADER_FUNCS)/%.c
	$(CC) -c $< -o $@

# Clean
clean:
	rm -f $(OBJ)/* $(BINDIR)/*
