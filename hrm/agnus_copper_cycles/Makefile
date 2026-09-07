# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP = agnus_copper
RTL = $(NANOMIG)/src/minimig-aga/agnus_copper.v
TEST_SRC = test_agnus_copper_cycles.cpp
OBJ_DIR = obj_dir
TEST_BIN = $(OBJ_DIR)/V$(TOP)

VERILATOR = verilator
FLAGS = --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal

.PHONY: all test clean

all: test

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(RTL)
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(RTL) $(TEST_SRC)

clean:
	rm -rf $(OBJ_DIR)
