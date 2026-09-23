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

# Prove every assertion can go red ALONE. MUTATE names one assertion's own
# label at a time (mutating(id) in the source); a green run for an id below
# means that assertion's expect() is not measuring what it claims to, and a
# red run that also takes down another id would mean the mutation is not
# isolated to the assertion it names. LONG-LINES-ARE-ONE-CLOCK-LONGER is the
# exception: it is red at baseline (a published finding), so its own
# mutation is inverted and its correct result here is GREEN, not red.
MUTATE_IDS := FIXTURE-RUNS-A-PROGRAMMED-LINE HTOTAL-IS-THE-HIGHEST-COUNT VARBEAMEN-GATES-THE-LIMIT-REGISTERS HTOTAL-IGNORES-THE-X-CELLS HSSTRT-AND-HSSTOP-BOUND-HSYNC HSSTRT-NEEDS-VARHSYEN-TOO HBSTRT-AND-HBSTOP-BOUND-BLANKING LACE-IS-BPLCON0-BIT-2-AND-RESET-CLEARS-IT LONG-LINE-TOGGLE-RUNS-UNLESS-PAL-OR-LOLDIS LONG-LINES-ARE-ONE-CLOCK-LONGER
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
