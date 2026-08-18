VERILATOR ?= verilator

TOP := agnus_copper
OBJ_DIR := obj_dir
RTL := ../../../src/minimig-aga/agnus_copper.v
TEST_SRC := test_agnus_copper_skip.cpp
TEST_BIN := $(OBJ_DIR)/V$(TOP)

.PHONY: all build test clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(RTL) $(TEST_SRC) Makefile
	$(VERILATOR) --trace --cc --exe --build -j 0 --Mdir $(OBJ_DIR) --top-module $(TOP) -Wall -Wno-fatal $(RTL) $(TEST_SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
