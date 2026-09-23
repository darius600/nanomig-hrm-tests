# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.
#
# -y on the source directory rather than a list of files: denise instantiates
# the colour table, the HAM generator, the bitplane shifters, the sprites and
# the collision unit, and Verilator finds them all from the library path. The
# blitter package lists five sources by hand; this is the shorter way and
# ch3-bplcon0-display-modes was already using it.

NANOMIG ?= ../../nanomig
SRC_DIR := $(NANOMIG)/src/minimig-aga

TOP := denise
SRC := $(SRC_DIR)/denise.v
TEST_SRC := test_denise.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

.PHONY: all test build mutate clean

all: test

build:
	$(VERILATOR) $(FLAGS) --top-module $(TOP) -y $(SRC_DIR) $(SRC) $(TEST_SRC) -o V$(TOP)

test: build
	./$(TEST_BIN)

# Prove every assertion can go red ALONE. MUTATE names one assertion's own
# label at a time (mutating(id) in the source); a green run for an id below
# means that assertion's expect() is not measuring what it claims to, and a
# red run that also takes down another id would mean the mutation is not
# isolated to the assertion it names.
MUTATE_IDS := FIXTURE-DELIVERS-KNOWN-VALUE LISAID-AT-07C LISAID-IDENTIFIES-THE-CHIP-SET COLOUR-TABLE-AT-180 BPLCON2-AT-104-OPENS-READBACK BPLCON3-AT-106-SELECTS-THE-BANK
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
