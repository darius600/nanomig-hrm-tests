# Where does the NanoMig checkout live? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP := agnus_blitter
SRC := $(NANOMIG)/src/minimig-aga/agnus_blitter.v \
       $(NANOMIG)/src/minimig-aga/agnus_blitter_barrelshifter.v \
       $(NANOMIG)/src/minimig-aga/agnus_blitter_fill.v \
       $(NANOMIG)/src/minimig-aga/agnus_blitter_minterm.v \
       $(NANOMIG)/src/minimig-aga/agnus_blitter_adrgen.v
TEST_SRC := test_agnus_blitter_ecs.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal

.PHONY: all test build clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
