CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O2

PROJECT=wav

SRC=main.c
OBJ_DIR=./obj
OBJ=$(addprefix $(OBJ_DIR)/,$(subst .c,.o,$(SRC)))

all:$(PROJECT)

$(PROJECT):$(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(PROJECT)
	mkdir -p $(OBJ_DIR)
