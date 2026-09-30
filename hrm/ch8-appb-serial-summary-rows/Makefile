# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := paula
# paula.v instantiates paula_uart, paula_intcontroller, paula_floppy and
# paula_audio directly (confirmed by reading paula.v's own instantiations);
# paula_audio in turn instantiates
# paula_audio_channel and paula_audio_mixer, which itself instantiates
# paula_audio_volume. paula_floppy in turn instantiates paula_floppy_fifo.
# paula_floppy_ebr.v and paula_floppy_dma_buf.v are deliberately NOT in this
# list: paula_floppy.v does not instantiate either of them, and
# paula_floppy_ebr.v declares a second `module paula_floppy` that must not
# be compiled alongside paula_floppy.v -- same SRC list as the sibling
# package hrm/ch8-adkcon-disk-control-bits, which also builds `paula` alone.
SRC := $(NANOMIG)/src/minimig-aga/paula.v \
       $(NANOMIG)/src/minimig-aga/paula_uart.v \
       $(NANOMIG)/src/minimig-aga/paula_intcontroller.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v \
       $(NANOMIG)/src/minimig-aga/paula_audio.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_channel.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_mixer.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_volume.v
TEST_SRC := test_ch8-appb-serial-summary-rows.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=MUT_<n> on the command line to build one mutation variant, e.g.
# `make MUT=MUT_7 test`. Unset/empty MUT builds the unmutated baseline.
# `test` always rebuilds from clean below (MUT is a #define, not a file, so
# make cannot see it as a dependency; without the unconditional
# `rm -rf $(OBJ_DIR)` here, `make MUT=X test` would silently reuse whatever
# binary -- baseline or a different variant -- happened to be sitting in
# $(OBJ_DIR) already and report its result as if it were MUT=X's).
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)

test: $(SRC) $(TEST_SRC)
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
