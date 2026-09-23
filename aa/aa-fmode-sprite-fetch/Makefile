# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.

NANOMIG ?= ../../nanomig

TOP := agnus_spritedma
SRC := $(NANOMIG)/src/minimig-aga/agnus_spritedma.v
TEST_SRC := test_agnus_spritedma.cpp
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
# isolated to the assertion it names.
MUTATE_IDS := FIXTURE-ADVANCES-A-POINTER FMODE-IS-DECODED-AT-1FC SPRITE-FETCH-ADVANCES-AS-THE-TABLE-SAYS FMODE-BITS-13-TO-4-DO-NOTHING SPRITE-FETCH-IGNORES-THE-BITPLANE-PAIR
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
