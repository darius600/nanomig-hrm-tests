# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := paula
# paula.v instantiates paula_uart, paula_intcontroller, paula_floppy and
# paula_audio directly (confirmed by reading paula.v's own instantiations,
# after SPECIFICATION.md was committed); paula_audio in turn instantiates
# paula_audio_channel and paula_audio_mixer, which itself instantiates
# paula_audio_volume. paula_floppy in turn instantiates paula_floppy_fifo
# (confirmed by reading paula_floppy.v). paula_floppy_ebr.v and
# paula_floppy_dma_buf.v are deliberately NOT in this list: paula_floppy.v
# does not instantiate either of them, and paula_floppy_ebr.v declares a
# second `module paula_floppy` (build brief rig hazard) that must not be
# compiled alongside paula_floppy.v.
SRC := $(NANOMIG)/src/minimig-aga/paula.v \
       $(NANOMIG)/src/minimig-aga/paula_uart.v \
       $(NANOMIG)/src/minimig-aga/paula_intcontroller.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v \
       $(NANOMIG)/src/minimig-aga/paula_audio.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_channel.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_mixer.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_volume.v
TEST_SRC := test_ch8-adkcon-disk-control-bits.cpp
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
