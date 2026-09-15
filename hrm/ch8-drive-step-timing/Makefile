# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := paula_floppy
# paula_floppy_ebr.v declares a SECOND `module paula_floppy` and must not be
# compiled alongside this file (finding_candidates.md, "one more rig
# hazard"). paula_floppy_dma_buf.v is not instantiated by this checkout of
# paula_floppy.v at all (the sector buffers are plain regs declared inline) --
# only paula_floppy_fifo.v is ever instantiated, so only it is compiled here.
SRC := $(NANOMIG)/src/minimig-aga/paula_floppy.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v
TEST_SRC := test_ch8-drive-step-timing.cpp
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
