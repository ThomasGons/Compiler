# ___MAKEFILE___

# Compiler, linker and flags

CC = gcc#-fsanitize=address
CFLAGS = -g -O3 -l -Wall -Wextra -std=c17 -I$(DINC)
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
	
$(TARGET): $(OBJ) $(CC_SUCCEED)
	@mkdir -p $(DBIN)
	$(CC) $^ $(LFLAGS) -o $@


$(DOBJ)%.o: $(DSRC)%.c
	@mkdir -p $(DOBJ)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean

clean:
	@rm -rf $(DOBJ) $(DBIN)
	@echo "Clean up project directories"

