# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := paula
SRC_DIR := $(NANOMIG)/src/minimig-aga
# paula.v instantiates paula_uart, paula_intcontroller, paula_floppy and
# paula_audio directly; paula_audio in turn instantiates four
# paula_audio_channel instances and one paula_audio_mixer, which itself
# instantiates four paula_audio_volume instances. Same file list as the
# only other package in this suite that already compiles paula.v
# (hrm/ch5-audio-dmacon-adkcon/Makefile) -- this package needs the whole
# module to elaborate, not just the seven files paula_floppy alone would
# need, because DMACON's own decode lives in paula.v itself.
SRC := $(SRC_DIR)/paula.v \
       $(SRC_DIR)/paula_uart.v \
       $(SRC_DIR)/paula_intcontroller.v \
       $(SRC_DIR)/paula_floppy.v \
       $(SRC_DIR)/paula_audio.v \
       $(SRC_DIR)/paula_audio_channel.v \
       $(SRC_DIR)/paula_audio_mixer.v \
       $(SRC_DIR)/paula_audio_volume.v
# paula_floppy_ebr.v and paula_floppy_dma_buf.v are deliberately NOT
# listed: paula.v itself does not instantiate either (confirmed by
# reading paula.v -- it instantiates paula_floppy only), so they are not
# part of this fixture's elaboration at all, not merely excluded as a
# workaround for the duplicate `module paula_floppy` in the EBR file.
TEST_SRC := test_ch8-dmaon-enable-composition.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
# paula_floppy.v instantiates paula_floppy_fifo (its own FIFO submodule),
# which is not in SRC above; -I lets Verilator find paula_floppy_fifo.v by
# module name automatically, matching hrm/ch5-audio-dmacon-adkcon/Makefile's
# own -I$(SRC_DIR) (its VERILOG_SOURCES list also omits paula_floppy_fifo.v
# for the same reason).
FLAGS += -I$(SRC_DIR)

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
