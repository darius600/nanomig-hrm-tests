# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.

NANOMIG ?= ../../nanomig

TOP := paula_floppy
# paula_floppy instantiates paula_floppy_fifo, so both files are named.
# paula_floppy_ebr.v declares a SECOND module called paula_floppy and
# must never be compiled beside these two.
SRC := $(NANOMIG)/src/minimig-aga/paula_floppy.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v
TEST_SRC := test_paula_floppy.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

.PHONY: all test build mutate clean

all: test

build:
	$(VERILATOR) $(FLAGS) --top-module $(TOP) $(SRC) $(TEST_SRC) -o V$(TOP)

test: build
	./$(TEST_BIN)

# Prove every assertion can go red ALONE -- or, for the two already red at
# baseline (they corroborate NM-HRM-041), prove the mutation can move them
# to green, which a red that could not be made to fail harder still shows.
# MUTATE names one assertion's own label at a time (mutating(id) in the
# source); a run that moves an assertion OTHER than the named one off its
# baseline verdict means the mutation is not isolated to the assertion it
# names.
MUTATE_IDS := FIXTURE-READS-A-LIVE-REGISTER DISKWRITE-MIRRORS-DSKLEN-WRITE DMAON-FOLLOWS-THE-DSKLEN-ENABLE DSKBYTR-BITS-11-TO-8-READ-ZERO DSKBYT-IS-CLEARED-BY-THE-READ WORDEQUAL-IS-TRUE-ONLY-WHILE-DSKSYNC-MATCHES
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
