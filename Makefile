CFLAGS = -g -O2 -Wall

INC_DIR := include
SRC_DIR := src
OUT_DIR := out
OBJ_DIR := objects

vpath %.c $(SRC_DIR)
vpath %.h $(INC_DIR)

sources := $(wildcard $(SRC_DIR)/*.c)
objects := $(sources:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: %.c %.h save_princesses_error.h 
	cc $(CFLAGS) -c $< -o $@

$(OUT_DIR)/save_princesses: $(objects) | $(OUT_DIR)
	cc $(CFLAGS) $^ -o $@

$(objects): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $@

$(OUT_DIR):
	mkdir $@

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(OUT_DIR)
