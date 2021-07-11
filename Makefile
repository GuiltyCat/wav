CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O2
LDFLAGS=-lm

PROJECT=wav

SRC=main.c
OBJ_DIR=./obj
OBJ=$(addprefix $(OBJ_DIR)/,$(subst .c,.o,$(SRC)))
DEP=$(subst .o,.d,$(OBJ))

.PHONY:all

all:$(PROJECT)

$(PROJECT):$(OBJ) 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(DEP)
$(OBJ_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

.PHONY:clean plot

clean:
	rm -rf $(OBJ_DIR) $(PROJECT)
	mkdir -p $(OBJ_DIR)
