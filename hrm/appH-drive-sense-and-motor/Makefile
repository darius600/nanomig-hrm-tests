# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := paula_floppy
# paula_floppy_ebr.v is deliberately NOT in this list: it declares a second
# `module paula_floppy` and must not be compiled alongside paula_floppy.v
# (see hrm/ch8-drive-sense-lines/SPECIFICATION.md, build brief rig hazard).
SRC := $(NANOMIG)/src/minimig-aga/paula_floppy.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v
TEST_SRC := test_appH-drive-sense-and-motor.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

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
