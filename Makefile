# ___MAKEFILE___

# Compiler, linker and flags

CC = gcc -fsanitize=address
CFLAGS = -g -O3 -l -Wall -Wextra 
LDFLAGS = -Wall -I$(DINC) -lm

# Directories

DSRC = src/
DINC = inc/
DCONFIG = config/
DOBJ = obj/
DBIN = bin/

# Files

SRC = $(wildcard $(DSRC)*.c)
INC = $(wildcard $(DINC)*.h)
OBJ = $(SRC:$(DSRC)%.c=$(DOBJ)%.o)

TARGET = $(DBIN)exe

all: $(TARGET)
	
$(TARGET): $(OBJ)
	@mkdir -p $(DBIN)
	$(CC) $^ $(LFLAGS) -o $@
	@echo "Linking finished !"

$(DOBJ)%.o: $(DSRC)%.c
	@mkdir -p $(DOBJ)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Successful compilation !"

.PHONY: clean clean_s

clean:
	@rm -rf $(DOBJ) $(DBIN)
	@echo "Clean up project directories"

clean_s:
	@rm -f $(OBJ) $(EXE)
	@echo "Objects and executable removed"

