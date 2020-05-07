CFLAGS = -g -O3 -Wall -I./include
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

tests_obj := tests.o
main_obj := save_princesses.o

$(OBJ_DIR)/%.o: %.c %.d
	$(CC) $(CFLAGS) -c $< -o $@ 

$(DEP_DIR)/%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

main: $(filter-out $(OBJ_DIR)/$(tests_obj), $(objects)) | $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $(OUT_DIR)/save_princesses

tests: $(filter-out $(OBJ_DIR)/$(main_obj), $(objects)) | $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $(OUT_DIR)/$@

include $(deps)

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
