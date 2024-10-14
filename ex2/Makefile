# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Executable names
EXEC1 = parastack_ex
EXEC2 = meter_passing
EXEC3 = temp_generator

# Source files
SRC1 = parastack_ex.c
SRC2 = meter_passing.c
SRC3 = temp_generator.c

# Object files
OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)
OBJ3 = $(SRC3:.c=.o)

# Default target to build all executables (can be run separately)
all: $(EXEC1) $(EXEC2) $(EXEC3)

# Target to compile parastack_ex separately
$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) -o $(EXEC1) $(OBJ1)

# Target to compile meter_passing separately
$(EXEC2): $(OBJ2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJ2)

# Target to compile temp_generator separately
$(EXEC3): $(OBJ3)
	$(CC) $(CFLAGS) -o $(EXEC3) $(OBJ3)

# Object file generation for parastack_ex
$(OBJ1): $(SRC1)
	$(CC) $(CFLAGS) -c $(SRC1)

# Object file generation for meter_passing
$(OBJ2): $(SRC2)
	$(CC) $(CFLAGS) -c $(SRC2)

# Object file generation for temp_generator
$(OBJ3): $(SRC3)
	$(CC) $(CFLAGS) -c $(SRC3)

# Clean up object files and executables
clean:
	rm -f $(OBJ1) $(OBJ2) $(OBJ3) $(EXEC1) $(EXEC2) $(EXEC3)
