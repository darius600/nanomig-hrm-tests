VERILATOR ?= verilator

TOP := agnus_blitter_line_octant
OBJ_DIR := obj_dir
RTL := agnus_blitter_line_octant.v
TEST_SRC := test_agnus_blitter_line_octant.cpp
TEST_BIN := $(OBJ_DIR)/V$(TOP)

.PHONY: all build test clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(RTL) $(TEST_SRC) Makefile
	$(VERILATOR) --cc --exe --build -j 0 --Mdir $(OBJ_DIR) --top-module $(TOP) -Wall -Wno-fatal $(RTL) $(TEST_SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
