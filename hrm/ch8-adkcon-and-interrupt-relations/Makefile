# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := paula
# paula.v instantiates paula_uart, paula_intcontroller, paula_floppy and
# paula_audio directly (paula.v:203,220,243,289); paula_audio in
# turn instantiates paula_audio_channel and paula_audio_mixer, which itself
# instantiates paula_audio_volume. paula_floppy in turn instantiates
# paula_floppy_fifo. paula_floppy_ebr.v and paula_floppy_dma_buf.v are
# deliberately NOT in this list, as in hrm/ch8-adkcon-disk-control-bits's
# Makefile: paula_floppy.v does not instantiate either of them, and
# paula_floppy_ebr.v declares a second `module paula_floppy` that must not
# be compiled alongside paula_floppy.v (Verilator, run with -Wno-fatal as
# here, only warns MODDUP and builds one of the two). This file list is the
# same one that package's Makefile uses.
SRC := $(NANOMIG)/src/minimig-aga/paula.v \
       $(NANOMIG)/src/minimig-aga/paula_uart.v \
       $(NANOMIG)/src/minimig-aga/paula_intcontroller.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v \
       $(NANOMIG)/src/minimig-aga/paula_audio.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_channel.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_mixer.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_volume.v
TEST_SRC := test_ch8-adkcon-and-interrupt-relations.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_3 test`. Unset/empty MUT builds the unmutated
# (baseline) binary. Each variant is a full separate build+run.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test clean

all: test

# `test` is phony and always runs its own rm -rf, deliberately NOT relying
# on $(TEST_BIN) as a file-target with $(SRC)/$(TEST_SRC) prerequisites: a
# file target's recipe is skipped by make whenever the binary is already
# newer than its listed prerequisites, which would silently reuse a STALE
# binary across `make MUT=X test` invocations (MUT is not a file, so it
# can never make a file target look out of date) and report the previous
# build's result as the new mutation's.
test: $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
