CFLAGS = -g -O2 -Wall -I./include
CC = gcc

SRC_DIR := src
OUT_DIR := out
OBJ_DIR := objects
DEP_DIR := deps

vpath %.d $(DEP_DIR)
vpath %.c $(SRC_DIR)
vpath %.o $(OBJ_DIR)

sources := $(wildcard $(SRC_DIR)/*.c)
objects := $(sources:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
deps := $(sources:$(SRC_DIR)/%.c=$(DEP_DIR)/%.d)

$(OBJ_DIR)/%.o: %.c %.d
	$(CC) $(CFLAGS) -c $< -o $@ 

$(DEP_DIR)/%.d: %.c
	$(CC) -MM $(CFLAGS) $< -o $@

$(OUT_DIR)/save_princesses: $(objects) | $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $@

-include $(deps)

$(objects): | $(OBJ_DIR)
$(deps): | $(DEP_DIR)

$(OBJ_DIR):
	mkdir $@

$(DEP_DIR):
	mkdir $@

$(OUT_DIR):
	mkdir $@

.PHONY: clean

clean:
	rm -rf $(DEP_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(OUT_DIR)
