# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := ciab
SRC := $(NANOMIG)/src/minimig-aga/ciab.v \
       $(NANOMIG)/src/minimig-aga/cia_int.v \
       $(NANOMIG)/src/minimig-aga/cia_timera.v \
       $(NANOMIG)/src/minimig-aga/cia_timerb.v \
       $(NANOMIG)/src/minimig-aga/cia_timerd.v
TEST_SRC := test_ch8-ciabprb-disk-control-bits.cpp
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
