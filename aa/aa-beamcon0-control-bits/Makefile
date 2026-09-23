# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.

NANOMIG ?= ../../nanomig

TOP := agnus_beamcounter
SRC := $(NANOMIG)/src/minimig-aga/agnus_beamcounter.v
TEST_SRC := test_agnus_beamcounter.cpp
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

# Prove every assertion can go red ALONE (or, for SYNC-POLARITY-BITS-CHANGE-
# THE-PINS, whose baseline is already red -- see NM-HRM-032 -- green). MUTATE
# names one assertion's own label at a time (mutating(id) in the source); a
# run that does not move that assertion's own verdict, or that moves another
# assertion's verdict too, means the mutation is not isolated to the one it
# names.
MUTATE_IDS := FIXTURE-DELIVERS-KNOWN-VALUE HARDDIS-AT-BIT-14 VARBEAMEN-AT-BIT-7 BEAMCON0-WRITE-IS-CHIPSET-GATED SYNC-POLARITY-BITS-CHANGE-THE-PINS
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
