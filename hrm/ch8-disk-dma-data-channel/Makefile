# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := disk_dma_data_channel_rig
SRC := disk_dma_data_channel_rig.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v \
       $(NANOMIG)/src/minimig-aga/agnus_diskdma.v
TEST_SRC := test_ch8-disk-dma-data-channel.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -Wno-UNUSED -Wno-WIDTH

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
