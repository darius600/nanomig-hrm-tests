# Where does the NanoMig checkout live? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

VERILATOR ?= verilator

TOP := agnus_blitter_adrgen
OBJ_DIR := obj_dir
RTL := $(NANOMIG)/src/minimig-aga/agnus_blitter_adrgen.v
TEST_SRC := test_agnus_blitter_adrgen.cpp
TEST_BIN := $(OBJ_DIR)/V$(TOP)

.PHONY: all build test clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(RTL) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) --cc --exe --build -j 0 --Mdir $(OBJ_DIR) --top-module $(TOP) -Wall -Wno-fatal $(RTL) $(TEST_SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
