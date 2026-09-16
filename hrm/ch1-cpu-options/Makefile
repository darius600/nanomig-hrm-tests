# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := sysctrl
# sysctrl.v is the whole fixture: it instantiates no submodule. It does
# $readmemh a menu ROM image ("amiga_xml.hex", sysctrl.v:89) at simulation
# start; none of this package's claims touch menu_rom_data/amiga_xml, but
# the hex file is copied alongside the binary below so that $readmemh does
# not have to search relative to an unrelated CWD.
SRC := $(NANOMIG)/src/misc/sysctrl.v
MENU_ROM := $(NANOMIG)/src/misc/amiga_xml.hex
TEST_SRC := test_ch1-cpu-options.cpp
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

.PHONY: all test build clean

all: test

build: $(TEST_BIN)

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
	cp $(MENU_ROM) $(OBJ_DIR)/amiga_xml.hex
	cd $(OBJ_DIR) && ./V$(TOP)

clean:
	rm -rf $(OBJ_DIR)
